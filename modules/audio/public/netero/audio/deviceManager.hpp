/**
 * Netero sources under BSD-3-Clause
 * see LICENSE.txt
 */

#pragma once

#include <memory>
#include <string>
#include <vector>

#include <netero/audio/device.hpp>
#include <netero/patterns/ISingleton.hpp>

namespace netero::audio {

class DeviceManager final: public patterns::ISingleton<DeviceManager> {
    public:
    enum class RtCode { SUCCESS = 0, NO_DEVICE_CONNECTED = 1 };

    ~DeviceManager() final;

    RtCode scanForDevices();

    std::vector<Device&>& getOutputDevices();
    std::vector<Device&>& getInputDevices();

    private:
    DeviceManager();

    class Impl;
    std::unique_ptr<Impl> _pImpl;
};
} // namespace netero::audio
