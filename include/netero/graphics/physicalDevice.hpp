/**
 * Netero sources under BSD-3-Clause
 * see LICENSE.txt
 */

#pragma once

#include <map>
#include <optional>
#include <string>
#include <memory>
#include <vulkan/vulkan.h>

namespace netero::graphics {

    struct QueueFamilyIndices {
        std::optional<uint32_t> graphicsFamily;

        [[nodiscard]] bool  isGraphicsSuitable() const {
            return graphicsFamily.has_value();
        }
    };

    class PhysicalDevice {
        PhysicalDevice() = default;
        static std::unique_ptr<PhysicalDevice> sInstance;
    public:
        static void     Initialize(VkInstance);
        static PhysicalDevice*  Instance();
        static void     Release();
        PhysicalDevice(const PhysicalDevice&) = delete;
        PhysicalDevice(PhysicalDevice&&) = delete;
        PhysicalDevice& operator=(const PhysicalDevice&) = delete;
        PhysicalDevice& operator=(PhysicalDevice&&) = delete;
        ~PhysicalDevice();

        [[nodiscard]] VkPhysicalDevice          getPhysicalDevice() const;
        [[nodiscard]] static int                rateDeviceSuitability(VkPhysicalDevice);
        [[nodiscard]] static QueueFamilyIndices findQueueFamilies(VkPhysicalDevice);

        [[nodiscard]] std::string getCurrentDeviceName() const;
        [[nodiscard]] std::multimap<int, std::string> getDevices() const;

    private:
        VkInstance          _instance = nullptr;
        VkPhysicalDevice    _physicalPhysicalDevice = nullptr;
        uint32_t            _deviceCount = 0;
    };
}

