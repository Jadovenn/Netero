/**
 * Netero sources under BSD-3-Clause
 * see LICENSE.txt
 */

#pragma once

#include <unordered_map>

#include <Netero/Audio/DeviceManager.hpp>

#include "DeviceImpl.hpp"

namespace Netero::Audio {
class Device;
}

class Netero::Audio::DeviceManager::Impl {
    public:
    Impl();
    ~Impl();

    DeviceManager::RtCode ScanForDevices();

    std::vector<std::unique_ptr<DeviceImpl>> outputDevices;
    std::vector<Device*>                     clientOutputDevices;

    std::vector<std::unique_ptr<DeviceImpl>> inputDevices;
    std::vector<Device*>                     clientInputDevices;
};
