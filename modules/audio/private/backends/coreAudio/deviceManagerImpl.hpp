/**
 * Netero sources under BSD-3-Clause
 * see LICENSE.txt
 */

#pragma once

#include <unordered_map>

#include <netero/audio/deviceManager.hpp>

#include "deviceImpl.hpp"

class netero::audio::DeviceManager::Impl {
    public:
    Impl();
    ~Impl();

    DeviceManager::RtCode scanForOutputDevice();

    std::unordered_map<unsigned, std::unique_ptr<DeviceImpl>> deviceMap;
};
