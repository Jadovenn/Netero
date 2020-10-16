/**
 * Netero sources under BSD-3-Clause
 * see LICENSE.txt
 */

#pragma once

#include <memory>
#include <string>
#include <vector>

#include <Netero/Audio/Device.hpp>
#include <Netero/Patterns/ISingleton.hpp>

namespace Netero::Audio {

class DeviceManager final: public Patterns::ISingleton<DeviceManager> {
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

    [[nodiscard]] bool IsValid();
    RtCode             ScanForDevices();

    const std::vector<Device*>& GetOutputDevices();
    const std::vector<Device*>& GetInputDevices();
    [[nodiscard]] int           GetDefaultOutputDeviceIdx();
    [[nodiscard]] int           GetDefaultInputDeviceIdx();

    private:
    class Impl;
    std::unique_ptr<Impl> myImpl;
};
} // namespace Netero::Audio
