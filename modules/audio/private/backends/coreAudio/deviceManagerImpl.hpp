/**
 * Netero sources under BSD-3-Clause
 * see LICENSE.txt
 */

#pragma once

#include <unordered_map>

#include <netero/audio/deviceManager.hpp>

#include "deviceImpl.hpp"

namespace netero::audio {
class Device;
}

class netero::audio::DeviceManager::Impl {
    public:
    Impl();
    ~Impl();

    DeviceManager::RtCode scanForOutputDevice();

    std::vector<std::unique_ptr<DeviceImpl>> outputDevices;
    std::vector<Device*>                     clientOutputDevices;

    std::vector<std::unique_ptr<DeviceImpl>> inputDevices;
    std::vector<Device*>                     clientInputDevices;
};
