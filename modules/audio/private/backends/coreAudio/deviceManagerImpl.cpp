/**
 * Netero sources under BSD-3-Clause
 * see LICENSE.txt
 */

#include "deviceManagerImpl.hpp"

netero::audio::DeviceManager::Impl::Impl()
{
}

netero::audio::DeviceManager::Impl::~Impl()
{
}

// DeviceManager Impl

netero::audio::DeviceManager::DeviceManager()
    : _pImpl(std::make_unique<netero::audio::DeviceManager::Impl>())
{
}

netero::audio::DeviceManager::~DeviceManager() noexcept = default;

bool netero::audio::DeviceManager::isValid()
{
    return true;
}

netero::audio::DeviceManager::RtCode netero::audio::DeviceManager::scanForDevices()
{
    return this->_pImpl->scanForDevices();
}

const std::vector<netero::audio::Device*>& netero::audio::DeviceManager::getOutputDevices()
{
    return this->_pImpl->clientOutputDevices;
}

const std::vector<netero::audio::Device*>& netero::audio::DeviceManager::getInputDevices()
{
    return this->_pImpl->clientInputDevices;
}

int netero::audio::DeviceManager::getDefaultOutputDeviceIdx()
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
    for (auto& device : this->_pImpl->outputDevices) {
        if (*device == id) {
            return idx;
        }
        idx += 1;
    }
    return -1;
}

int netero::audio::DeviceManager::getDefaultInputDeviceIdx()
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
    for (auto& device : this->_pImpl->inputDevices) {
        if (*device == id) {
            return id;
        }
    }
    return -1;
}
