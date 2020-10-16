/**
 * Netero sources under BSD-3-Clause
 * see LICENSE.txt
 */

#include "DeviceManagerImpl.hpp"

namespace Netero::Audio {

DeviceManager::Impl::Impl()
{
}

DeviceManager::Impl::~Impl()
{
}

// DeviceManager Impl

DeviceManager::DeviceManager(): myImpl(std::make_unique<DeviceManager::Impl>())
{
}

DeviceManager::~DeviceManager() noexcept = default;

bool DeviceManager::IsValid()
{
    return true;
}

DeviceManager::RtCode DeviceManager::ScanForDevices()
{
    return this->myImpl->ScanForDevices();
}

const std::vector<Device*>& DeviceManager::GetOutputDevices()
{
    return this->myImpl->clientOutputDevices;
}

const std::vector<Device*>& DeviceManager::GetInputDevices()
{
    return this->myImpl->clientInputDevices;
}

int DeviceManager::GetDefaultOutputDeviceIdx()
{
    UInt32                     size;
    AudioObjectPropertyAddress property = { kAudioHardwarePropertyDevices,
                                            kAudioObjectPropertyScopeGlobal,
                                            kAudioObjectPropertyElementMaster };
    OSStatus                   result =
        AudioObjectGetPropertyDataSize(kAudioObjectSystemObject, &property, 0, nullptr, &size);
    if (result != noErr) {
        return -1;
    }
    AudioDeviceID id;
    UInt32        audioDeviceSize = sizeof(AudioDeviceID);
    property.mScope = kAudioHardwarePropertyDefaultOutputDevice;
    result = AudioObjectGetPropertyData(kAudioObjectSystemObject,
                                        &property,
                                        0,
                                        nullptr,
                                        &audioDeviceSize,
                                        &id);
    if (result != noErr) {
        return -1;
    }
    int idx = 0;
    for (auto& device : this->myImpl->outputDevices) {
        if (*device == id) {
            return idx;
        }
        idx += 1;
    }
    return -1;
}

int DeviceManager::GetDefaultInputDeviceIdx()
{
    UInt32                     size;
    AudioObjectPropertyAddress property = { kAudioHardwarePropertyDevices,
                                            kAudioObjectPropertyScopeOutput,
                                            kAudioObjectPropertyElementMaster };
    OSStatus                   result =
        AudioObjectGetPropertyDataSize(kAudioObjectSystemObject, &property, 0, nullptr, &size);
    if (result != noErr) {
        return -1;
    }
    AudioDeviceID id;
    UInt32        audioDeviceSize = sizeof(AudioDeviceID);
    property.mScope = kAudioHardwarePropertyDefaultInputDevice;
    result = AudioObjectGetPropertyData(kAudioObjectSystemObject,
                                        &property,
                                        0,
                                        nullptr,
                                        &audioDeviceSize,
                                        &id);
    if (result != noErr) {
        return -1;
    }
    int idx = 0;
    for (auto& device : this->myImpl->inputDevices) {
        if (*device == id) {
            return idx;
        }
        idx += 1;
    }
    return -1;
}

} // namespace Netero::Audio