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
	unsigned device = _device_id;
	WaveFormat	format{};
	CFStringRef cf_str;

	//======================================
	// Check if a device is open
	//======================================
	if (!_is_device_open) {
		return WaveFormat {};
	}

	//======================================
	// Get the current device id
	//======================================
	unsigned int nDevices = CORE_AUDIO_get_device_count();
	if ( nDevices == 0 ) {
		throw std::runtime_error("netero_audio: There is no devices.");
	}

	if ( device >= nDevices ) {
		throw std::runtime_error("netero_audio: Wrong device id.");
	}

	AudioDeviceID deviceList[ nDevices ];
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
	if (result != noErr) {
		throw std::runtime_error("netero_audio: Error while retrieving the devices IDs.");
	}
	AudioDeviceID id = deviceList[ device ];

	//======================================
	// Get the device manufacturer name
	//======================================
	dataSize = sizeof(CFStringRef);
	property.mSelector = kAudioObjectPropertyManufacturer;
	result = AudioObjectGetPropertyData(
			id,
			&property,
			0,
			nullptr,
			&dataSize,
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
			&dataSize,
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
	dataSize = 0;
	result = AudioObjectGetPropertyDataSize(
			id,
			&property,
			0,
			nullptr,
			&dataSize);
	if (result != noErr || dataSize == 0) {
		std::string msg("netero_audio: Error("
						+ std::to_string(static_cast<unsigned>(result))
						+ ") while getting stream configuration");
		throw std::runtime_error(msg);
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
			bufferList );
	if ( result != noErr || dataSize == 0 ) {
		delete[] bufferList;
		std::string msg("netero_audio: Error("
						+ std::to_string(static_cast<unsigned>(result))
						+ ") while getting stream configuration for device " + std::to_string(device));
		throw std::runtime_error(msg);
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
			&dataSize );
	if ( result != kAudioHardwareNoError || dataSize == 0 ) {
		std::string msg("netero_audio: Error("
						+ std::to_string(static_cast<unsigned>(result))
						+ ") while getting sampling rate on device " + std::to_string(device));
		throw std::runtime_error(msg);
	}
	UInt32 nRanges = dataSize / sizeof( AudioValueRange );
	AudioValueRange rangeList[ nRanges ];
	result = AudioObjectGetPropertyData(
			id,
			&property,
			0,
			nullptr,
			&dataSize,
			&rangeList);
	if ( result != kAudioHardwareNoError ) {
		std::string msg("netero_audio: Error("
						+ std::to_string(static_cast<unsigned>(result))
						+ ") while getting sampling rate on device " + std::to_string(device));
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
		for (auto samplingRate: _samplingRates) {
			if (samplingRate >= (unsigned)minimumRate && samplingRate <= (unsigned)maximumRate) {
				format.supportedSamplingRate.push_back(samplingRate);
			}
		}
	}
	auto value = std::find_if(std::begin(_samplingRates), std::end(_samplingRates), [] (float val) {
		return val == 48000;
	});
	if (value != std::end(_samplingRates)) {
		format.samplingFrequency = *value;
	}
	else {
		format.samplingFrequency = *std::max(std::begin(_samplingRates), std::end(_samplingRates));
	}
	std::sort( format.supportedSamplingRate.begin(), format.supportedSamplingRate.end() );
	format.supportedSamplingRate.erase(std::unique(format.supportedSamplingRate.begin(), format.supportedSamplingRate.end()),
									   format.supportedSamplingRate.end());
	if (format.supportedSamplingRate.empty()) {
		std::string msg("netero_audio: Error no supported sampling rate found on device " + std::to_string(device));
		throw std::runtime_error(msg);
	}
	return format;
}
