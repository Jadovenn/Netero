/**
 * Netero sources under BSD-3-Clause
 * see LICENSE.txt
 */

#pragma once

#include <memory>
#include <vector>

#include <netero/audio/deviceManager.hpp>

#include "deviceImpl.hpp"

extern const CLSID CLSID_MMDeviceEnumerator;
extern const IID   IID_IMMDeviceEnumerator;

class netero::audio::DeviceManager::Impl {
    public:
    Impl();
    ~Impl();

    DeviceManager::RtCode scanForOutputDevice();

    IMMDeviceEnumerator* deviceEnumerator;

    std::vector<std::unique_ptr<DeviceImpl>> internalOuputDevices;
    std::vector<Device&>                     clientOutputDevices;

    std::vector<std::unique_ptr<DeviceImpl>> internalInputDevices;
    std::vector<Device&>                     clientInputDevices;
};
