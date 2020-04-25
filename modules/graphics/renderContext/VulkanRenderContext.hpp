/**
 * Netero sources under BSD-3-Clause
 * see LICENSE.txt
 */

#pragma once

#include <vulkan/vulkan.h>

#include <vector>
#include <netero/graphics/renderContext.hpp>

namespace netero::graphics {
    class RenderContext::impl {
    public:
        impl(const std::string& appName, bool enableValidationLayers);
        impl(const impl&) = delete;
        impl(impl&&) = delete;
        impl& operator=(const impl&) = delete;
        impl& operator=(impl&&) = delete;
        ~impl();

        // Render init proc
        void createVulkanContext();
        [[nodiscard]] std::vector<const char*> getRequiredExtensions() const;
        [[nodiscard]] bool checkValidationLayerSupport() const;
        void setupDebugMessenger();
        void setupDebugReportCallback();
        static void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT&);
        static void populateDebugReportCreateInfo(VkDebugReportCallbackCreateInfoEXT&);

        // Render release proc
        void releaseVulkanContext() const;

        const std::string               appName;
        bool                            enableValidationLayers;
        static const std::vector<char*> validationLayers;

        VkInstance          instance;
        VkApplicationInfo   appInfo = {};
        VkDebugUtilsMessengerEXT    debugMessenger = nullptr;
        VkDebugReportCallbackEXT    debugReport = nullptr;
    };
}

