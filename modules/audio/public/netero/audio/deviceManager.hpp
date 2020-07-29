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
    private:
    DeviceManager();
    DECLARE_SINGLETON(DeviceManager);

    public:
    enum class RtCode { SUCCESS = 0, NO_DEVICE_CONNECTED = 1, SYSTEM_API_ERROR = 2 };

    DeviceManager(const DeviceManager&) = delete;
    DeviceManager(DeviceManager&&) = delete;
    DeviceManager& operator=(const DeviceManager&) = delete;
    DeviceManager& operator=(DeviceManager&&) = delete;
    ~DeviceManager() noexcept final;

    [[nodiscard]] bool isValid();
    RtCode             scanForDevices();

    const std::vector<Device*>& getOutputDevices();
    const std::vector<Device*>& getInputDevices();

    private:
    class Impl;
    std::unique_ptr<Impl> _pImpl;
};
} // namespace netero::audio
