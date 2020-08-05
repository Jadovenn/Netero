/**
 * Netero sources under BSD-3-Clause
 * see LICENSE.txt
 */

#pragma once

#include <string>

#include <vulkan/vulkan.h>

namespace netero::graphics {

class Device {
    VkInstance   _instance;
    VkSurfaceKHR _surface;
    int          _width;
    int          _height;

    void pickPhysicalDevice();
    void createLogicalDevice(VkPhysicalDevice);
    void createTransferCommandPool();

    public:
    Device(VkInstance, VkSurfaceKHR);
    Device(const Device&) = delete;
    Device(Device&&) = delete;
    Device& operator=(const Device&) = delete;
    Device& operator=(Device&&) = delete;
    ~Device();
    [[nodiscard]] VkSurfaceKHR getAssociatedSurface() const;
    [[nodiscard]] int          getSurfaceWidth() const;
    [[nodiscard]] int          getSurfaceHeight() const;
    void                       setSurfaceWidth(const int);
    void                       setSurfaceHeight(const int);

    VkPhysicalDevice physicalDevice;
    VkDevice         logicalDevice;
    VkQueue          graphicsQueue;
    VkQueue          presentQueue;
    VkQueue          transferQueue;
    VkCommandPool    transferCommandPool;
    std::string      deviceName;
};

} // namespace netero::graphics
