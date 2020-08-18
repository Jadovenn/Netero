/**
 * Netero sources under BSD-3-Clause
 * see LICENSE.txt
 */

#pragma once

#include <vector>

#include <Vulkan/Vulkan.hpp>

namespace VkUtils {

#pragma region Validation layers

extern const std::vector<const char*> ValidationLayers;

std::vector<const char*> GetRequiredExtensions();
bool                     CheckValidationLayerSupport();

void PopulateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT&);
void PopulateDebugReportCreateInfo(VkDebugReportCallbackCreateInfoEXT&);

VkResult CreateDebugMessengerEXT(VkInstance,
                                 const VkDebugUtilsMessengerCreateInfoEXT*,
                                 const VkAllocationCallbacks* pAllocator,
                                 VkDebugUtilsMessengerEXT*);

void DestroyDebugMessengerEXT(VkInstance, VkDebugUtilsMessengerEXT, const VkAllocationCallbacks*);

VkResult CreateDebugReportEXT(VkInstance,
                              const VkDebugReportCallbackCreateInfoEXT*,
                              const VkAllocationCallbacks*,
                              VkDebugReportCallbackEXT*);

void DestroyDebugReportEXT(VkInstance, VkDebugReportCallbackEXT, const VkAllocationCallbacks*);

VKAPI_ATTR VkBool32 VKAPI_CALL
DebugMessengerCallback(VkDebugUtilsMessageSeverityFlagBitsEXT      messageSeverity,
                       VkDebugUtilsMessageTypeFlagsEXT             messageType,
                       const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
                       void*                                       pUserData);

VKAPI_ATTR VkBool32 VKAPI_CALL DebugReportCallback(VkFlags                    msgFlags,
                                                   VkDebugReportObjectTypeEXT objType,
                                                   uint64_t                   srcObject,
                                                   size_t                     location,
                                                   int32_t                    msgCode,
                                                   const char*                pLayerPrefix,
                                                   const char*                pMsg,
                                                   void*                      pUserData);
} // namespace VkUtils
