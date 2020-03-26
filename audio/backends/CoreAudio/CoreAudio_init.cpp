/**
 * Netero sources under BSD-3-Clause
 * see LICENCE.txt
 */

#include <iostream>
#include <exception>
#include <vector>
#include <algorithm>
#include <CoreAudio/AudioHardware.h>

#include "CoreAudio.hpp"

void netero::audio::engine::impl::CORE_AUDIO_init() {
	// Necessary code to make CoreAudio working properly
	CFRunLoopRef theRunLoop = nullptr;
	AudioObjectPropertyAddress property = { kAudioHardwarePropertyRunLoop,
											kAudioObjectPropertyScopeGlobal,
											kAudioObjectPropertyElementMaster };
	OSStatus result = AudioObjectSetPropertyData( kAudioObjectSystemObject,
			&property,
			0,
			nullptr,
			sizeof(CFRunLoopRef),
			&theRunLoop);
	if (result != noErr) {
		throw std::runtime_error("netero_audio: error setting run loop property!");
	}
}

void netero::audio::engine::impl::CORE_AUDIO_release() {
}

unsigned netero::audio::engine::impl::CORE_AUDIO_get_device_count() {
	UInt32 size;
	AudioObjectPropertyAddress propertyAddress = {
			kAudioHardwarePropertyDevices,
			kAudioObjectPropertyScopeGlobal,
			kAudioObjectPropertyElementMaster
	};
	OSStatus result = AudioObjectGetPropertyDataSize(
			kAudioObjectSystemObject,
			&propertyAddress,
			0,
			nullptr,
			&size );
	if (result != noErr) {
		throw std::runtime_error("netero_audio: Error retrieving device info.");
	}
	return size / sizeof( AudioDeviceID );
}

unsigned netero::audio::engine::impl::CORE_AUDIO_get_default_output_device() {
	unsigned devices_count = CORE_AUDIO_get_device_count();
	if (devices_count <= 1) {
		return 0;
	}
	AudioDeviceID id;
	UInt32 audioDeviceSize = sizeof( AudioDeviceID );
	AudioObjectPropertyAddress property = {
			kAudioHardwarePropertyDefaultOutputDevice,
			kAudioObjectPropertyScopeGlobal,
			kAudioObjectPropertyElementMaster
	};
	OSStatus result = AudioObjectGetPropertyData(
			kAudioObjectSystemObject,
			&property,
			0,
			nullptr,
			&audioDeviceSize,
			&id);
	if (result != noErr) {
		throw std::runtime_error("netero_audio: Error retrieving default device");
	}

	audioDeviceSize = sizeof( AudioDeviceID ) * devices_count;
	AudioDeviceID deviceList[ devices_count ];
	property.mSelector = kAudioHardwarePropertyDevices;
	result = AudioObjectGetPropertyData(
			kAudioObjectSystemObject,
			&property,
			0,
			nullptr,
			&audioDeviceSize,
			reinterpret_cast<void*>(&deviceList));
	if (result != noErr) {
		throw std::runtime_error("netero_audio: Error retrieving default device");
	}
	for (unsigned idx = 0; idx < devices_count; idx++ ) {
		if (id == deviceList[idx]) {
			return idx;
		}
	}
	throw std::runtime_error("netero_audio: No default device found");
}

size_t netero::audio::engine::impl::getBufferSize() {
	return 0;
}
