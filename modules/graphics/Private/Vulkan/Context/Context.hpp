/**
 * Netero sources under BSD-3-Clause
 * see LICENSE.txt
 */

#pragma once

#include <Vulkan/Vulkan.hpp>

namespace Netero::Gfx {

class Context {
    public:
    Context();

    VkInstance   myVulkanInstance;
    VkSurfaceKHR mySurface;

    int myWidth;
    int myHeight;

    VkPhysicalDevice myPhysicalDevice;
    VkDevice         myLogicalDevice;

    VkQueue myGraphicsQueue;
    VkQueue myPresentQueue;
    VkQueue myTransferQueue;
    VkCommandPool myTransferCommandPool;
};

} // namespace Netero::Gfx
