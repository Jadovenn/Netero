/**
 * Netero sources under BSD-3-Clause
 * see LICENSE.txt
 */

#pragma once

#include <Vulkan/Vulkan.hpp>

namespace Netero::Gfx {

class Context {
    public:
    private:
    VkInstance   myVulkanInstance;
    VkSurfaceKHR mySurface;
    int          myWidth;
    int          myHeight;

    VkPhysicalDevice myPhysicalDevice;
    VkDevice         myLogicalDevice;
};

} // namespace Netero::Gfx
