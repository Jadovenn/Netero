/**
 * Netero sources under BSD-3-Clause
 * see LICENSE.txt
 */

#pragma once

#include <vulkan/vulkan.h>

namespace vkUtils {
    VkResult    CreateDebugUtilsMessengerEXT(VkInstance,
        const VkDebugUtilsMessengerCreateInfoEXT*,
        const VkAllocationCallbacks* pAllocator,
        VkDebugUtilsMessengerEXT*);

    void        DestroyDebugUtilsMessengerEXT(VkInstance,
        VkDebugUtilsMessengerEXT,
        const VkAllocationCallbacks*);

    VkResult    CreateDebugReportEXT(VkInstance,
        const VkDebugReportCallbackCreateInfoEXT*,
        const VkAllocationCallbacks*,
        VkDebugReportCallbackEXT*);

    void DestroyDebugReportEXT(VkInstance,
        VkDebugReportCallbackEXT,
        const VkAllocationCallbacks*);

    VKAPI_ATTR VkBool32 VKAPI_CALL debugMessengerCallback(
        VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
        VkDebugUtilsMessageTypeFlagsEXT messageType,
        const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
        void* pUserData);

    VKAPI_ATTR VkBool32 VKAPI_CALL debugReportCallback(VkFlags msgFlags,
        VkDebugReportObjectTypeEXT objType,
        uint64_t srcObject,
        size_t location,
        int32_t msgCode,
        const char* pLayerPrefix,
        const char* pMsg,
        void* pUserData);
}

