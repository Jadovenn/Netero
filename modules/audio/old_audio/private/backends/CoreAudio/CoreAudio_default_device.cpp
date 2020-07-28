/**
 * Netero sources under BSD-3-Clause
 * see LICENCE.txt
 */

#include <algorithm>
#include <exception>

#include "CoreAudio.hpp"

#include <CoreAudio/AudioHardware.h>

unsigned netero::audio::engine::impl::CORE_AUDIO_get_device_count()
{
    UInt32                     size;
    AudioObjectPropertyAddress propertyAddress = { kAudioHardwarePropertyDevices,
                                                   kAudioObjectPropertyScopeGlobal,
                                                   kAudioObjectPropertyElementMaster };
    OSStatus                   result = AudioObjectGetPropertyDataSize(kAudioObjectSystemObject,
                                                     &propertyAddress,
                                                     0,
                                                     nullptr,
                                                     &size);
    if (result != noErr) {
        throw std::runtime_error("netero_audio: Error retrieving device info.");
    }
    return size / sizeof(AudioDeviceID);
}

netero::audio::RtCode
netero::audio::engine::impl::CORE_AUDIO_get_default_output_device(Device &buffer)
{
    unsigned devices_count = CORE_AUDIO_get_device_count();
    buffer.deviceId = 0;
    if (devices_count <= 1) {
        return RtCode::ERR_NATIVE;
    }
    AudioDeviceID              id;
    UInt32                     audioDeviceSize = sizeof(AudioDeviceID);
    AudioObjectPropertyAddress property = { kAudioHardwarePropertyDefaultOutputDevice,
                                            kAudioObjectPropertyScopeGlobal,
                                            kAudioObjectPropertyElementMaster };
    OSStatus                   result = AudioObjectGetPropertyData(kAudioObjectSystemObject,
                                                 &property,
                                                 0,
                                                 nullptr,
                                                 &audioDeviceSize,
                                                 &id);
    if (result != noErr) {
        throw std::runtime_error("netero_audio: Error retrieving default device");
    }

    audioDeviceSize = sizeof(AudioDeviceID) * devices_count;
    AudioDeviceID deviceList[devices_count];
    property.mSelector = kAudioHardwarePropertyDevices;
    result = AudioObjectGetPropertyData(kAudioObjectSystemObject,
                                        &property,
                                        0,
                                        nullptr,
                                        &audioDeviceSize,
                                        reinterpret_cast<void *>(&deviceList));
    if (result != noErr) {
        return RtCode::ERR_NATIVE;
    }
    for (auto device : deviceList) {
        if (id == device) {
            buffer.deviceId = device;
        }
    }
    if (buffer.deviceId == 0) {
        return RtCode::ERR_NATIVE;
    }
    return RtCode::OK;
}

size_t netero::audio::engine::impl::getBufferSize()
{
    auto format = getFormat();
    return format.framesCount;
}
