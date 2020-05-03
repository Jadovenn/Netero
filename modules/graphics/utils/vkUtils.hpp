/**
 * Netero sources under BSD-3-Clause
 * see LICENSE.txt
 */

#pragma once

#include <string>
#include <vector>
#include <map>
#include <optional>
#include <vulkan/vulkan.h>

namespace vkUtils {
    // Global variable
    extern const std::vector<char*> validationLayers;
    extern const std::vector<const char*> defaultDeviceExtensions;

    // Queue Family related
    struct QueueFamilyIndices {
        std::optional<uint32_t> graphicsFamily;
        std::optional<uint32_t> presentFamily;

        [[nodiscard]] bool  isGraphicsSuitable() const {
            return graphicsFamily.has_value() && presentFamily.has_value();
        }
    };
    bool                                checkDeviceSuitable(VkPhysicalDevice, const std::vector<const char*>);
    [[nodiscard]] QueueFamilyIndices    findQueueFamilies(VkPhysicalDevice, VkSurfaceKHR);

    // Physical Device related
    [[nodiscard]] static int            rateDeviceSuitability(VkPhysicalDevice, VkSurfaceKHR);
    [[nodiscard]] std::multimap<int, VkPhysicalDevice>      getRatedAvailableDevices(VkInstance, VkSurfaceKHR);
    [[nodiscard]] VkPhysicalDevice                          getBestDevice(VkInstance, VkSurfaceKHR);
    [[nodiscard]] std::string   getDeviceName(VkPhysicalDevice);
    [[nodiscard]] VkPhysicalDevice  getDeviceByName(const char*, VkInstance);
    [[nodiscard]] std::vector<std::string>   getDevicesName(VkInstance);

    // Swap chain related
    struct SwapChainSupportDetails {
        VkSurfaceCapabilitiesKHR        capabilities;
        std::vector<VkSurfaceFormatKHR> formats;
        std::vector<VkPresentModeKHR>   presentMode;
    };

    SwapChainSupportDetails QuerySwapChainSupport(VkPhysicalDevice, VkSurfaceKHR);

    // Validation layers related
    std::vector<const char*>    getRequiredExtensions();
    bool                        checkValidationLayerSupport();

    void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT&);
    void populateDebugReportCreateInfo(VkDebugReportCallbackCreateInfoEXT&);

    VkResult    CreateDebugMessengerEXT(VkInstance,
        const VkDebugUtilsMessengerCreateInfoEXT*,
        const VkAllocationCallbacks* pAllocator,
        VkDebugUtilsMessengerEXT*);

    void        DestroyDebugMessengerEXT(VkInstance,
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

