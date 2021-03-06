/**
 * Netero sources under BSD-3-Clause
 * see LICENSE.txt
 */

#define GLFW_INCLUDE_VULKAN
#include <string>
#include <vector>

#include <GLFW/glfw3.h>
#include <Vulkan/VkUtils.hpp>

#include <Netero/Logger.hpp>
#include <Netero/Netero.hpp>

namespace VkUtils {

const std::vector<const char*> ValidationLayers = { "VK_LAYER_KHRONOS_validation" };

bool CheckValidationLayerSupport()
{
    uint32_t layerCount;
    vkEnumerateInstanceLayerProperties(&layerCount, nullptr);
    std::vector<VkLayerProperties> availableLayers(layerCount);
    vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());
    LOG_INFO << "Available Vulkan validation layers:" << std::endl;
    for (auto& prop : availableLayers) {
        LOG_INFO << "\t" << prop.layerName << std::endl;
    }
    for (auto& layerName : ValidationLayers) {
        auto it = std::find_if(availableLayers.begin(),
                               availableLayers.end(),
                               [layerName](VkLayerProperties& prop) -> bool {
                                   return !std::strcmp(layerName, prop.layerName);
                               });
        if (it == availableLayers.end()) {
            LOG_INFO << "Error missing layer: " << layerName << std::endl;
            return false;
        }
    }
    return true;
}

std::vector<const char*> GetRequiredExtensions()
{
    uint32_t extensionCount = 0;
    uint32_t glfwExtensionCount = 0;

    vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);
    std::vector<VkExtensionProperties> extensions(extensionCount);
    vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, extensions.data());
    if (Netero::IsDebugMode) {
        LOG_INFO << "Available Vulkan extensions:" << std::endl;
        for (const auto& ext : extensions) {
            LOG_INFO << "\t" << ext.extensionName << std::endl;
        }
    }
    const char** glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
    std::vector<const char*> extension(glfwExtensions, glfwExtensions + glfwExtensionCount);
    for (unsigned idx = 0; idx < glfwExtensionCount; ++idx) {
        auto it = std::find_if(extensions.begin(), extensions.end(), [&](const auto ext) -> bool {
            return strcmp(glfwExtensions[idx], ext.extensionName) == 0;
        });
        if (it == extensions.end()) {
            throw std::runtime_error("Could not create instance, missing extension: " +
                                     std::string(glfwExtensions[idx]));
        }
        if (Netero::IsDebugMode) {
            LOG_INFO << "Using extensions: " << glfwExtensions[idx] << std::endl;
        }
    }
    if (Netero::IsDebugMode) {
        extension.push_back(VK_EXT_DEBUG_REPORT_EXTENSION_NAME);
        extension.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
    }
    return extension;
}

VkResult CreateDebugMessengerEXT(VkInstance                                instance,
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

void DestroyDebugMessengerEXT(VkInstance                   instance,
                              VkDebugUtilsMessengerEXT     debugMessenger,
                              const VkAllocationCallbacks* pAllocator)
{
    const auto function = reinterpret_cast<PFN_vkDestroyDebugUtilsMessengerEXT>(
        vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT"));
    if (function != nullptr) {
        function(instance, debugMessenger, pAllocator);
    }
}

VkResult CreateDebugReportEXT(VkInstance                                instance,
                              const VkDebugReportCallbackCreateInfoEXT* createInfo,
                              const VkAllocationCallbacks*              allocator,
                              VkDebugReportCallbackEXT*                 debugReportCallback)
{
    const auto function = reinterpret_cast<PFN_vkCreateDebugReportCallbackEXT>(
        vkGetInstanceProcAddr(instance, "vkCreateDebugReportCallbackEXT"));
    if (function != nullptr) {
        return function(instance, createInfo, allocator, debugReportCallback);
    }
    return VK_ERROR_EXTENSION_NOT_PRESENT;
}

void DestroyDebugReportEXT(VkInstance                   instance,
                           VkDebugReportCallbackEXT     debugReportCallback,
                           const VkAllocationCallbacks* pAllocator)
{
    const auto function = reinterpret_cast<PFN_vkDestroyDebugReportCallbackEXT>(
        vkGetInstanceProcAddr(instance, "vkDestroyDebugReportCallbackEXT"));
    if (function != nullptr) {
        function(instance, debugReportCallback, pAllocator);
    }
}

} // namespace VkUtils
