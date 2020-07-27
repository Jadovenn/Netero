/**
 * Netero sources under BSD-3-Clause
 * see LICENSE.txt
 */

#pragma once

#include <memory>

#include <netero/patterns/ISingleton.hpp>

namespace netero::audio {

class DeviceManager final: public patterns::ISingleton<DeviceManager> {
    public:
    enum class RtCode { SUCCESS = 0, NO_DEVICE_CONNECTED = 1};

    ~DeviceManager() final = default;

    RtCode scanForDevices();

    private:
    DeviceManager();

    class Impl;
    std::unique_ptr<Impl> _pImpl;
};
} // namespace netero::audio
