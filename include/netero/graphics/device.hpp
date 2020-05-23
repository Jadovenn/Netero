/**
 * Netero sources under BSD-3-Clause
 * see LICENSE.txt
 */

#pragma once

#include <string>
#include <vulkan/vulkan.h>

namespace netero::graphics {

    struct Device {
    private:
        VkInstance      _instance;
        VkSurfaceKHR    _surface;

        void pickPhysicalDevice();
        void createLogicalDevice(VkPhysicalDevice);
    public:
        Device(VkInstance, VkSurfaceKHR);

        VkPhysicalDevice    physicalDevice;
        VkDevice            logicalDevice;
        VkQueue             graphicsQueue;
        VkQueue             presentQueue;
        std::string         deviceName;
    };
}

