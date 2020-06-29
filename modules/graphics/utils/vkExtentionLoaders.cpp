/**
 * Netero sources under BSD-3-Clause
 * see LICENSE.txt
 */

#include "vkUtils.hpp"

VkResult vkUtils::CreateDebugMessengerEXT(VkInstance                                instance,
                                          const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo,
                                          const VkAllocationCallbacks*              pAllocator,
                                          VkDebugUtilsMessengerEXT*                 pDebugMessenger)
{
    const auto function = reinterpret_cast<PFN_vkCreateDebugUtilsMessengerEXT>(
        vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT"));
    if (function != nullptr) {
        return function(instance, pCreateInfo, pAllocator, pDebugMessenger);
    }
    return VK_ERROR_EXTENSION_NOT_PRESENT;
}

void vkUtils::DestroyDebugMessengerEXT(VkInstance                   instance,
                                       VkDebugUtilsMessengerEXT     debugMessenger,
                                       const VkAllocationCallbacks* pAllocator)
{
    const auto function = reinterpret_cast<PFN_vkDestroyDebugUtilsMessengerEXT>(
        vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT"));
    if (function != nullptr) {
        function(instance, debugMessenger, pAllocator);
    }
}

VkResult vkUtils::CreateDebugReportEXT(VkInstance                                instance,
                                       const VkDebugReportCallbackCreateInfoEXT* createInfo,
                                       const VkAllocationCallbacks*              allocator,
                                       VkDebugReportCallbackEXT* debugReportCallback)
{
    const auto function = reinterpret_cast<PFN_vkCreateDebugReportCallbackEXT>(
        vkGetInstanceProcAddr(instance, "vkCreateDebugReportCallbackEXT"));
    if (function != nullptr) {
        return function(instance, createInfo, allocator, debugReportCallback);
    }
    return VK_ERROR_EXTENSION_NOT_PRESENT;
}

void vkUtils::DestroyDebugReportEXT(VkInstance                   instance,
                                    VkDebugReportCallbackEXT     debugReportCallback,
                                    const VkAllocationCallbacks* pAllocator)
{
    const auto function = reinterpret_cast<PFN_vkDestroyDebugReportCallbackEXT>(
        vkGetInstanceProcAddr(instance, "vkDestroyDebugReportCallbackEXT"));
    if (function != nullptr) {
        function(instance, debugReportCallback, pAllocator);
    }
}
