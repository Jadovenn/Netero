/**
 * Netero sources under BSD-3-Clause
 * see LICENCE.txt
 */

#include <CoreAudio/AudioHardware.h>

#include "CoreAudio.hpp"

static std::string 	cf_string_to_std_string(CFStringRef &cf_str) {
	int length = CFStringGetLength(cf_str);
	char *c_str = new (std::nothrow) char[(length * 3 + 1)];
	if (!c_str) {
		throw std::bad_alloc();
	}
#if defined( UNICODE ) || defined( _UNICODE )
	CFStringGetCString(cf_str, c_str, length * 3 + 1, kCFStringEncodingUTF8);
#else
	CFStringGetCString(cf_str, c_str, length * 3 + 1, CFStringGetSystemEncoding());
#endif
	std::string	str(c_str);
	delete[] c_str;
	return str;
}

netero::audio::WaveFormat netero::audio::engine::impl::getFormat() {
	WaveFormat	format{};
	CFStringRef cf_str;
	OSStatus result = 0;
	UInt32 size = 0;
	AudioObjectPropertyAddress property = {
			kAudioHardwarePropertyDevices,
			kAudioObjectPropertyScopeGlobal,
			kAudioObjectPropertyElementMaster
	};

	//======================================
	// Check if a device is open
	//======================================
	if (_device.deviceId == 0) {
		return format;
	}

	//======================================
	// Get the current device id
	//======================================
	AudioDeviceID id = _device.deviceId;

	//======================================
	// Get the device manufacturer name
	//======================================
	size = sizeof(CFStringRef);
	property.mSelector = kAudioObjectPropertyManufacturer;
	result = AudioObjectGetPropertyData(
			id,
			&property,
			0,
			nullptr,
			&size,
			&cf_str);
	if ( result != noErr ) {
		std::string msg("netero_audio: Error("
						+ std::to_string(static_cast<unsigned>(result))
						+ ") while getting device manufacturer name");
		throw std::runtime_error(msg);
	}
	format.manufacturer = cf_string_to_std_string(cf_str);
	CFRelease(cf_str);

	//======================================
	// Get the device name
	//======================================
	property.mSelector = kAudioObjectPropertyName;
	result = AudioObjectGetPropertyData(
			id,
			&property,
			0,
			nullptr,
			&size,
			&cf_str);
	if ( result != noErr ) {
		std::string msg("netero_audio: Error("
						+ std::to_string(static_cast<unsigned>(result))
						+ ") while getting device name");
		throw std::runtime_error(msg);
	}
	format.name += cf_string_to_std_string(cf_str);
	CFRelease(cf_str);

	//======================================
	// Retrieve the device properties
	//======================================
	AudioBufferList	*bufferList = nullptr;
	property.mSelector = kAudioDevicePropertyStreamConfiguration;
	property.mScope = kAudioDevicePropertyScopeOutput;
	size = 0;
	result = AudioObjectGetPropertyDataSize(
			id,
			&property,
			0,
			nullptr,
			&size);
	if (result != noErr || size == 0) {
		std::string msg("netero_audio: Error("
						+ std::to_string(static_cast<unsigned>(result))
						+ ") while getting stream configuration");
		return format;
	}
	bufferList = new (std::nothrow) AudioBufferList[size];
	if (!bufferList) {
		throw std::bad_alloc();
	}
	result = AudioObjectGetPropertyData(
			id,
			&property,
			0,
			nullptr,
			&size,
			bufferList );
	if ( result != noErr || size == 0 ) {
		delete[] bufferList;
		std::string msg("netero_audio: Error("
						+ std::to_string(static_cast<unsigned>(result))
						+ ") while getting stream configuration for device " + std::to_string(_device.deviceId));
		return format;
	}
	unsigned nStreams = bufferList->mNumberBuffers;
	for (unsigned idx = 0; idx < nStreams; idx++) {
		format.channels = bufferList->mBuffers[idx].mNumberChannels;
		format.framesCount = bufferList->mBuffers[idx].mDataByteSize;
	}
	format.bytesPerSample = sizeof(float);
	format.bytesPerFrame = sizeof(float) * format.channels;
	delete[] bufferList;

	//======================================
	// Retrieve supported sampling rate
	// - Retrieve the supported min and max rate,
	//   then translate to a uniform list of
	//   supported rate, then choose 48000Hz rate
	//   or the higher rate by default
	//======================================
	property.mSelector = kAudioDevicePropertyAvailableNominalSampleRates;
	property.mScope = kAudioDevicePropertyScopeOutput;
	result = AudioObjectGetPropertyDataSize(
			id,
			&property,
			0,
			nullptr,
			&size );
	if ( result != kAudioHardwareNoError || size == 0 ) {
		std::string msg("netero_audio: Error("
						+ std::to_string(static_cast<unsigned>(result))
						+ ") while getting sampling rate on device " + std::to_string(_device.deviceId));
		return format;
	}
	UInt32 nRanges = size / sizeof( AudioValueRange );
	AudioValueRange rangeList[ nRanges ];
	result = AudioObjectGetPropertyData(
			id,
			&property,
			0,
			nullptr,
			&size,
			&rangeList);
	if ( result != kAudioHardwareNoError ) {
		std::string msg("netero_audio: Error("
						+ std::to_string(static_cast<unsigned>(result))
						+ ") while getting sampling rate on device " + std::to_string(_device.deviceId));
		throw std::runtime_error(msg);
	}
	Float64 minimumRate = 1.0, maximumRate = 10000000000.0;
	bool haveValueRange = false;
	std::vector<float> sampleRates;
	for (unsigned  idx = 0; idx < nRanges; idx++) {
		if ( rangeList[idx].mMinimum == rangeList[idx].mMaximum ) {
			auto samplingRate = (float) rangeList[idx].mMinimum;
			format.supportedSamplingRate.push_back(samplingRate);
			format.samplingFrequency = samplingRate;
		}
		else {
			haveValueRange = true;
			if (rangeList[idx].mMinimum > minimumRate) { minimumRate = rangeList[idx].mMinimum; }
			if (rangeList[idx].mMaximum < maximumRate) { maximumRate = rangeList[idx].mMaximum; }
		}
	}
	if (haveValueRange) {
		for (auto samplingRate: SamplingRates) {
			if (samplingRate >= (unsigned)minimumRate && samplingRate <= (unsigned)maximumRate) {
				format.supportedSamplingRate.push_back(samplingRate);
			}
		}
	}
	Float64 nominalRate;
	size = sizeof( Float64 );
	property.mSelector = kAudioDevicePropertyNominalSampleRate;
	result = AudioObjectGetPropertyData(
			id,
			&property,
			0,
			nullptr,
			&size,
			&nominalRate );
	if ( result != noErr ) {
		std::string msg ("netero_audio: Error while retrieving the sampling rate.");
		return format;
	}
	format.samplingFrequency = nominalRate;

	std::sort( format.supportedSamplingRate.begin(), format.supportedSamplingRate.end() );
	format.supportedSamplingRate.erase(std::unique(format.supportedSamplingRate.begin(), format.supportedSamplingRate.end()),
									   format.supportedSamplingRate.end());
	if (format.supportedSamplingRate.empty()) {
		std::string msg("netero_audio: Error no supported sampling rate found on device " + std::to_string(_device.deviceId));
		return format;
	}
	return format;
}
