/**
 * Netero sources under BSD-3-Clause
 * see LICENSE.txt
 */

#pragma once

#include <vulkan/vulkan.h>


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

