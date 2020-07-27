/**
 * Netero sources under BSD-3-Clause
 * see LICENSE.txt
 */

#include "deviceManagerImpl.hpp"

#include <CoreAudio/AudioHardware.h>
#include <CoreAudio/CoreAudio.h>

static UInt32 GetDeviceCount()
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

netero::audio::DeviceManager::RtCode netero::audio::DeviceManager::Impl::scanForOutputDevice()
{
    UInt32 deviceCount = GetDeviceCount();
    if (deviceCount == 0) {
        return RtCode::NO_DEVICE_CONNECTED;
    }

    AudioObjectPropertyAddress property = { kAudioHardwarePropertyDevices,
                                            kAudioObjectPropertyScopeGlobal,
                                            kAudioObjectPropertyElementMaster };
    UInt32                     deviceSize = sizeof(AudioDeviceID) * deviceCount;
    AudioDeviceID              deviceIDList[deviceCount];
    OSStatus                   result = AudioObjectGetPropertyData(kAudioObjectSystemObject,
                                                 &property,
                                                 0,
                                                 nullptr,
                                                 &deviceSize,
                                                 reinterpret_cast<void*>(&deviceIDList));
    if (result != noErr) {
        return RtCode::NO_DEVICE_CONNECTED;
    }
    for (auto deviceID : deviceIDList) {
        this->deviceMap[deviceID] = std::make_unique<DeviceImpl>(deviceID);
    }
    return RtCode::SUCCESS;
}
