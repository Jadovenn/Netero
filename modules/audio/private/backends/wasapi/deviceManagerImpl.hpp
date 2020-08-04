/**
 * Netero sources under BSD-3-Clause
 * see LICENSE.txt
 */

#pragma once

#include <memory>
#include <vector>

#include <netero/audio/deviceManager.hpp>

#include "deviceImpl.hpp"

class netero::audio::DeviceManager::Impl {
    public:
    Impl();
    ~Impl();

    DeviceManager::RtCode scanForOutputDevice();
    DeviceManager::RtCode scanForInputDevice();

    std::vector<std::unique_ptr<DeviceImpl>> outputDevices;
    std::vector<Device*>                     clientOutputDevices;

    std::vector<std::unique_ptr<DeviceImpl>> inputDevices;
    std::vector<Device*>                     clientInputDevices;

    bool                 isValid;
    IMMDeviceEnumerator* deviceEnumerator;
};
