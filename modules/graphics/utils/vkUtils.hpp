/**
 * Netero sources under BSD-3-Clause
 * see LICENSE.txt
 */

#pragma once

#include <string>
#include <vector>
#include <map>
#include <utility>
#include <optional>
#include <vulkan/vulkan.h>

namespace netero::graphics {
    class Device;
}

namespace vkUtils {
    // Global variable
    extern const std::vector<const char*> validationLayers;
    extern const std::vector<const char*> defaultDeviceExtensions;

    // Queue Family related
    struct QueueFamilyIndices {
        std::optional<uint32_t> graphicsFamily;
        std::optional<uint32_t> presentFamily;
        std::optional<uint32_t> transferFamily;

        [[nodiscard]] bool  isGraphicsSuitable() const {
            return graphicsFamily.has_value() && presentFamily.has_value();
        }
    };
    bool                                checkDeviceSuitable(VkPhysicalDevice, const std::vector<const char*>);
    [[nodiscard]] QueueFamilyIndices    findQueueFamilies(VkPhysicalDevice, VkSurfaceKHR);

    // Physical Device related
    [[nodiscard]] int                                       rateDeviceSuitability(VkPhysicalDevice, VkSurfaceKHR);
    [[nodiscard]] std::multimap<int, VkPhysicalDevice>      getRatedAvailableDevices(VkInstance, VkSurfaceKHR);
    [[nodiscard]] VkPhysicalDevice          getBestDevice(VkInstance, VkSurfaceKHR);
    [[nodiscard]] std::string               getDeviceName(VkPhysicalDevice);
    [[nodiscard]] VkPhysicalDevice          getDeviceByName(const char*, VkInstance);
    [[nodiscard]] std::vector<std::string>  getDevicesName(VkInstance);

    // Swap chain related
    struct SwapChainSupportDetails {
        VkSurfaceCapabilitiesKHR        capabilities;
        std::vector<VkSurfaceFormatKHR> formats;
        std::vector<VkPresentModeKHR>   presentMode;
    };

    SwapChainSupportDetails QuerySwapChainSupport(VkPhysicalDevice, VkSurfaceKHR);
    VkSurfaceFormatKHR      ChooseSwapSurfaceFormat(std::vector<VkSurfaceFormatKHR>&);
    VkPresentModeKHR        ChooseSwapPresentMode(std::vector<VkPresentModeKHR>&);
    VkExtent2D              ChooseSwapExtent(const VkSurfaceCapabilitiesKHR&, uint32_t, uint32_t);

    // Memory related
    int32_t                             FindMemoryType(VkPhysicalDevice physicalDevice, uint32_t typeFilter, VkMemoryPropertyFlags properties);
    std::pair<VkBuffer, VkDeviceMemory> AllocBuffer(netero::graphics::Device*, VkDeviceSize, VkBufferUsageFlags, VkMemoryPropertyFlags);
    std::pair<VkImage, VkDeviceMemory>  AllocImage(netero::graphics::Device*, uint32_t, uint32_t, VkFormat, VkImageTiling, VkImageUsageFlags, VkMemoryPropertyFlags);

    // Transfer and Command Buffer
    VkCommandBuffer BeginCommandBufferRecording(VkDevice, VkCommandPool, VkCommandBufferUsageFlags);
    void            TerminateCommandBufferRecording(VkDevice, VkQueue, VkCommandPool, VkCommandBuffer);
    void            FlushCommandBuffer(VkDevice, VkQueue, VkCommandPool, VkCommandBuffer);

    void    TransferBuffer(netero::graphics::Device* device, VkBuffer source, VkBuffer destination, VkDeviceSize size);
    void    TransitionImageLayout(netero::graphics::Device* device, VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout);
    void    TransferImage(netero::graphics::Device* device, VkBuffer source, VkImage destination, uint32_t width, uint32_t height);

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

