/**
 * Netero sources under BSD-3-Clause
 * see LICENSE.txt
 */

#include "utils/vkUtils.hpp"

vkUtils::SwapChainSupportDetails vkUtils::QuerySwapChainSupport(VkPhysicalDevice device, VkSurfaceKHR surface) {
    vkUtils::SwapChainSupportDetails    details;

    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface, &details.capabilities);

    uint32_t    formatCount = 0;
    vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, nullptr);
    if (formatCount != 0) {
        details.formats.resize(formatCount);
        vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, details.formats.data());
    }

    uint32_t    presentModeCount = 0;
    vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, nullptr);
    if (presentModeCount != 0) {
        details.presentMode.resize(presentModeCount);
        vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, details.presentMode.data());
    }
    return details;
}

