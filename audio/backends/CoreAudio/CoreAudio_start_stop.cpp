/**
 * Netero sources under BSD-3-Clause
 * see LICENCE.txt
 */

#include <chrono>
#include <thread>
#include <CoreAudio/AudioHardware.h>

#include "CoreAudio.hpp"

OSStatus	netero::audio::engine::impl::callbackHandler( AudioDeviceID inDevice,
		const AudioTimeStamp*,
		const AudioBufferList*,
		const AudioTimeStamp*,
		AudioBufferList* outOutputData,
		const AudioTimeStamp*,
		void* infoPointer )
{
	auto *device = reinterpret_cast<Device*>(infoPointer);
	if (device->deviceId == inDevice) {
		if (device->buffer.callBack) {
			auto buffer = reinterpret_cast<float*>(outOutputData->mBuffers[device->buffer.id].mData);
			size_t frames = outOutputData->mBuffers[device->buffer.id].mDataByteSize / device->format.bytesPerFrame;
			std::memset(buffer, 0, outOutputData->mBuffers[device->buffer.id].mDataByteSize);
			device->buffer.callBack(buffer, frames);
		}
	}
	return kAudioHardwareNoError;
}

static OSStatus rateListener(AudioObjectID inDevice,
							  UInt32,
							  const AudioObjectPropertyAddress*,
							  void* ratePointer)
{
	auto *rate = (Float64 *) ratePointer;
	UInt32 data_size = sizeof(Float64);
	AudioObjectPropertyAddress property = {
			kAudioDevicePropertyNominalSampleRate,
			kAudioObjectPropertyScopeGlobal,
			kAudioObjectPropertyElementMaster
	};
	AudioObjectGetPropertyData(
			inDevice,
			&property,
			0,
			nullptr,
			&data_size,
			rate);
	return kAudioHardwareNoError;
}

netero::audio::RtCode netero::audio::engine::impl::start() {

	if (!_cb) {
		return RtCode::ERR_MISSING_CALLBACK;
	}
	_device.buffer.callBack = _cb;
	if (_ioProcId) {
		return RtCode::ERR_ALREADY_RUNNING;
	}
	_device.format = getFormat();
	//==================================
	// Open the default audio device
	//==================================
	unsigned nDevices = CORE_AUDIO_get_device_count();
	if ( nDevices == 0) {
		_device.deviceId = 0;
		std::string msg("netero_audio: Panic, no audio device found while default asked.");
		return RtCode::ERR_NATIVE;
	}

	AudioDeviceID deviceList[nDevices];
	UInt32 dataSize = sizeof( AudioDeviceID ) * nDevices;
	AudioObjectPropertyAddress property = {
			kAudioHardwarePropertyDevices,
			kAudioObjectPropertyScopeGlobal,
			kAudioObjectPropertyElementMaster
	};
	OSStatus result = AudioObjectGetPropertyData(
			kAudioObjectSystemObject,
			&property,
			0,
			nullptr,
			&dataSize,
			reinterpret_cast<void*>(&deviceList));
	if ( result != noErr ) {
		throw std::runtime_error("netero_audio: Panic, could not get audio devices id.");
	}

	//==================================
	// Open the default audio device
	// - this is useful to do this again in case
	// I want to support different config later
	//==================================
	//AudioDeviceID id = deviceList[ device ];
	AudioDeviceID id = _device.deviceId;
	property.mScope = kAudioDevicePropertyScopeOutput;
	AudioBufferList	*bufferList = nullptr;
	dataSize = 0;
	property.mSelector = kAudioDevicePropertyStreamConfiguration;
	result = AudioObjectGetPropertyDataSize(
			id,
			&property,
			0,
			nullptr,
			&dataSize);
	if ( result != noErr || dataSize == 0 ) {
		std::string msg("netero_audio: Error("
						+ std::to_string(static_cast<unsigned>(result))
						+ ") could not retrieve stream configuration for device " + std::to_string(_device.deviceId));
		return RtCode::ERR_NATIVE;
	}
	bufferList = new (std::nothrow) AudioBufferList[dataSize];
	if (!bufferList) {
		throw std::bad_alloc();
	}
	result = AudioObjectGetPropertyData(
			id,
			&property,
			0,
			nullptr,
			&dataSize,
			bufferList);
	if (result != noErr || dataSize == 0) {
		delete[] bufferList;
		std::string msg("netero_audio: Error("
						+ std::to_string(static_cast<unsigned>(result))
						+ ") could not retrieve stream configuration for device " + std::to_string(_device.deviceId));
		return RtCode::ERR_NATIVE;
	}
	int channel_index = -1;
	for (unsigned idx = 0; idx < bufferList->mNumberBuffers; idx++) {
		if (bufferList->mBuffers[idx].mNumberChannels >= _device.format.channels) {
			channel_index = (int)idx;
			break;
		}
	}

	if (channel_index < 0) {
		delete[] bufferList;
		return RtCode::FORMAT_NOT_SUPPORTED;
	}
	delete[] bufferList;
	_device.buffer.id = channel_index;

	//==================================
	// Determinate buffer size
	//==================================
	AudioValueRange	bufferRange;
	dataSize = sizeof( AudioValueRange );
	property.mSelector = kAudioDevicePropertyBufferFrameSizeRange;
	result = AudioObjectGetPropertyData(
			id,
			&property,
			0,
			nullptr,
			&dataSize,
			&bufferRange );
	if ( result != noErr ) { // error getting the buffer ize range for the device
		return RtCode::ERR_NATIVE;
	}
	auto buffer_frame_size = static_cast<UInt32>(bufferRange.mMaximum);
	dataSize = sizeof( UInt32 );
	property.mSelector = kAudioDevicePropertyBufferFrameSize;
	result = AudioObjectSetPropertyData(
			id,
			&property,
			0,
			nullptr,
			dataSize,
			&buffer_frame_size);
	if ( result != noErr ) { // error while setting the buffer size
		return RtCode::ERR_NATIVE;
	}

	//==================================
	// Set the sampling rate
	//==================================
	Float64 nominalRate;
	dataSize = sizeof( Float64 );
	property.mSelector = kAudioDevicePropertyNominalSampleRate;
	result = AudioObjectGetPropertyData(
			id,
			&property,
			0,
			nullptr,
			&dataSize,
			&nominalRate );
	if ( result != noErr ) { // Error while getting the current sample rate
		return RtCode::ERR_NATIVE;
	}
	if (nominalRate != _device.format.samplingFrequency) { // if device sampling rate different, reconfigure it
		// here start the tricky thing, the rate is setup by a callback
		// So we register a call back, ask to change the rate, wait until the rate change
		// And finally unregister the callback
		Float64 reportedRate = 0.0;
		AudioObjectPropertyAddress cb_properties = {
				kAudioDevicePropertyNominalSampleRate,
				kAudioObjectPropertyScopeGlobal,
				kAudioObjectPropertyElementMaster
		};
		result = AudioObjectAddPropertyListener(
				id,
				&cb_properties,
				rateListener,
				reinterpret_cast<void*>(&reportedRate));
		if ( result != noErr ) { // Error could not register the callback
			return RtCode::ERR_NATIVE;
		}
		nominalRate = (Float64)_device.format.samplingFrequency;
		result = AudioObjectSetPropertyData(
				id,
				&property,
				0,
				nullptr,
				dataSize,
				&nominalRate);
		if ( result != noErr ) { // Error could not set new sampling rate
			AudioObjectRemovePropertyListener(
					id,
					&cb_properties,
					rateListener,
					reinterpret_cast<void*>(&reportedRate));
			return RtCode::ERR_NATIVE;
		}
		std::chrono::time_point	start = std::chrono::system_clock::now();
		std::chrono::time_point ref = start;
		while (reportedRate != nominalRate) {
			ref = std::chrono::system_clock::now();
			if (std::chrono::duration_cast<std::chrono::microseconds>(ref - start).count() < 5000) {
				break;
			}
			else {
				std::this_thread::yield();
			}
		}
		AudioObjectRemovePropertyListener(
				id,
				&cb_properties,
				rateListener,
				reinterpret_cast<void*>(&reportedRate));
		if (std::chrono::duration_cast<std::chrono::microseconds>(ref - start).count() < 5000) {
			return RtCode::DEVICE_TIMEOUT;
		}
	}

	AudioStreamBasicDescription	description;
	dataSize = sizeof( AudioStreamBasicDescription );
	property.mSelector = kAudioStreamPropertyVirtualFormat;
	result = AudioObjectGetPropertyData(
			id,
			&property,
			0,
			nullptr,
			&dataSize,
			&description);
	if ( result != noErr ) { // Error getting stream format
		return RtCode::ERR_NATIVE;
	}
	description.mSampleRate = (Float64)_device.format.samplingFrequency;
	description.mFormatID = kAudioFormatLinearPCM;
	result = AudioObjectSetPropertyData(
			id,
			&property,
			0,
			nullptr,
			dataSize,
			&description);
	if ( result != noErr ) { // Error while setting stream data rate and format
		return RtCode::ERR_NATIVE;
	}

	property.mSelector = kAudioStreamPropertyPhysicalFormat;
	result = AudioObjectGetPropertyData(
			id,
			&property,
			0,
			nullptr,
			&dataSize,
			&description);
	if (result != noErr) { // Error while getting physical device format
		return RtCode::ERR_NATIVE;
	}

	//==============================================
	// Check that the physical format match our need
	//==============================================

	if ( description.mFormatID != kAudioFormatLinearPCM || description.mBitsPerChannel < 16 ) {
		description.mFormatID = kAudioFormatLinearPCM;
		UInt32 formatFlags;

		std::vector<std::pair<UInt32, UInt32>>	physicalFormats;
		formatFlags = (description.mFormatFlags | kLinearPCMFormatFlagIsFloat) & ~kLinearPCMFormatFlagIsSignedInteger;
		physicalFormats.emplace_back( std::pair<Float32, UInt32>( 32, formatFlags ) ); // PCM 32 bits float
		formatFlags = (description.mFormatFlags | kLinearPCMFormatFlagIsSignedInteger | kAudioFormatFlagIsPacked) & ~kLinearPCMFormatFlagIsFloat;
		physicalFormats.emplace_back( std::pair<Float32, UInt32>( 32, formatFlags ) ); // PCM 32 bits packed signed integer
		physicalFormats.emplace_back( std::pair<Float32, UInt32>( 24, formatFlags ) ); // PCM 24 bits packed signed integer
		formatFlags &= ~( kAudioFormatFlagIsPacked | kAudioFormatFlagIsAlignedHigh );
		physicalFormats.emplace_back( std::pair<Float32, UInt32>( 24.2, formatFlags ) ); // PCM 24 bits over 4bytes, aligned low
		formatFlags |= kAudioFormatFlagIsAlignedHigh;
		physicalFormats.emplace_back( std::pair<Float32, UInt32>( 24.4, formatFlags ) ); // PCM 24 bits over 4bytes, aligned high
		formatFlags = (description.mFormatFlags | kLinearPCMFormatFlagIsSignedInteger | kAudioFormatFlagIsPacked) & ~kLinearPCMFormatFlagIsFloat;
		physicalFormats.emplace_back( std::pair<Float32, UInt32>( 16, formatFlags ) ); // PCM 16 bits signed integer
		physicalFormats.emplace_back( std::pair<Float32, UInt32>( 8, formatFlags ) ); // PCM 8 bits signed integer

		bool setPhysicalFormat = false;
		for (auto &format: physicalFormats) {
			AudioStreamBasicDescription testDescription = description;
			testDescription.mBitsPerChannel = static_cast<UInt32>(format.first);
			testDescription.mFormatFlags = format.second;
			if ( (24 == (UInt32)format.first) && ~( format.second & kAudioFormatFlagIsPacked ) ) {
				testDescription.mBytesPerFrame = 4 * testDescription.mChannelsPerFrame;
			}
			else {
				testDescription.mBytesPerFrame =
						testDescription.mBitsPerChannel / 8 * testDescription.mChannelsPerFrame;
			}
			testDescription.mBytesPerPacket = testDescription.mBytesPerFrame * testDescription.mFramesPerPacket;
			result = AudioObjectSetPropertyData(
					id,
					&property,
					0,
					nullptr,
					dataSize,
					&testDescription );
			if ( result == noErr ) {
				setPhysicalFormat = true;
				break;
			}
		}

		if ( !setPhysicalFormat ) {
			std::string msg("netero_audio: Error could not set physical device format");
			return RtCode::ERR_NATIVE;
		}
	}

	UInt32 latency;
	dataSize = sizeof(UInt32);
	property.mSelector = kAudioDevicePropertyLatency;
	if (AudioObjectHasProperty(id, &property)) {
		result = AudioObjectGetPropertyData(
				id,
				&property,
				0,
				nullptr,
				&dataSize,
				&latency);
		if ( result == kAudioHardwareNoError ) {
			(void)latency; // Idk what to do with this, for sure this is not normal so I let it here until I find out.
		}
		else {
			std::string msg("netero_audio: Error while retrieving physical device latency.");
			return RtCode::ERR_NATIVE;
		}
	}
	result = AudioDeviceCreateIOProcID(
			id,
			callbackHandler,
			reinterpret_cast<void*>(&_device),
			&_device.buffer.callBackId);
	if ( result != noErr ) {
		std::string msg("netero_audio: Error while registering device callback.");
		_device.buffer.reset();
		return RtCode::ERR_NATIVE;
	}
	result = AudioDeviceStart(_device.deviceId, _device.buffer.callBackId);
	if (result != noErr) {
		std::string msg("netero_audio: Error while starting the device.");
		_device.buffer.reset();
		return RtCode::ERR_NATIVE;
	}
	return RtCode::OK;
}

netero::audio::RtCode netero::audio::engine::impl::stop() {
	AudioDeviceStop( _device.deviceId, _device.buffer.callBackId);
	AudioDeviceDestroyIOProcID( _device.deviceId, _device.buffer.callBackId);
	_device.buffer.reset();
	return RtCode::OK;
}

netero::audio::RtCode netero::audio::engine::impl::poll() {
	return RtCode::OK;
}
