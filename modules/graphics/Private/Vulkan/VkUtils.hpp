/**
 * Netero sources under BSD-3-Clause
 * see LICENSE.txt
 */

#pragma once

#include <map>
#include <optional>
#include <vector>

#include <Vulkan/Vulkan.hpp>

namespace Netero::Gfx {
class Context;
}

namespace VkUtils {

struct QueueFamilyIndices {
    std::optional<uint32_t> graphicsFamily;
    std::optional<uint32_t> presentFamily;
    std::optional<uint32_t> transferFamily;

    [[nodiscard]] bool isGraphicsSuitable() const
    {
        return graphicsFamily.has_value() && presentFamily.has_value();
    }
};

extern const std::vector<const char*> DefaultDeviceExtensions;

#pragma region Device related

QueueFamilyIndices FindQueueFamilies(VkPhysicalDevice device, VkSurfaceKHR surface);
bool CheckDeviceSuitable(VkPhysicalDevice device, const std::vector<const char*> extensions);
int  RateDeviceSuitability(VkPhysicalDevice device, VkSurfaceKHR surface);
std::multimap<int, VkPhysicalDevice> GetRatedAvailableDevices(VkInstance   instance,
                                                              VkSurfaceKHR surface);
VkPhysicalDevice                     GetBestDevice(VkInstance instance, VkSurfaceKHR surface);
std::string                          GetDeviceName(VkPhysicalDevice device);
VkPhysicalDevice                     GetDeviceByName(const char* name, VkInstance instance);
std::vector<std::string>             GetDevicesName(VkInstance instance);

#pragma region Memory Helpers

int32_t FindMemoryType(VkPhysicalDevice      physicalDevice,
                       uint32_t              typeFilter,
                       VkMemoryPropertyFlags properties);

std::pair<VkBuffer, VkDeviceMemory> AllocBuffer(Netero::Gfx::Context& aContext,
                                                VkDeviceSize          size,
                                                VkBufferUsageFlags    usages,
                                                VkMemoryPropertyFlags properties);

std::pair<VkImage, VkDeviceMemory> AllocImage(Netero::Gfx::Context& aContext,
                                              uint32_t              width,
                                              uint32_t              height,
                                              VkFormat              format,
                                              VkImageTiling         tiling,
                                              VkImageUsageFlags     usages,
                                              VkMemoryPropertyFlags properties);

#pragma region Image Helpers

VkImageView
CreateImageView(VkDevice device, VkImage image, VkFormat format, VkImageAspectFlags aspectFlags);
VkFormat SelectSupportedImageFormat(VkPhysicalDevice             physicalDevice,
                                    const std::vector<VkFormat>& candidates,
                                    VkImageTiling                tiling,
                                    VkFormatFeatureFlags         features);
VkFormat FindDepthBufferingImageFormat(VkPhysicalDevice physicalDevice);
bool HasStencilComponent(VkFormat format);

#pragma region Swapchain Support Helpers

struct SwapChainSupportDetails {
    VkSurfaceCapabilitiesKHR        capabilities;
    std::vector<VkSurfaceFormatKHR> formats;
    std::vector<VkPresentModeKHR>   presentMode;
};

SwapChainSupportDetails QuerySwapChainSupport(VkPhysicalDevice, VkSurfaceKHR);
VkSurfaceFormatKHR      ChooseSwapSurfaceFormat(std::vector<VkSurfaceFormatKHR>&);
VkPresentModeKHR        ChooseSwapPresentMode(std::vector<VkPresentModeKHR>&);
VkExtent2D              ChooseSwapExtent(const VkSurfaceCapabilitiesKHR&, uint32_t, uint32_t);

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
