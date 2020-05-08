/**
 * Netero sources under BSD-3-Clause
 * see LICENSE.txt
 */

#include <stdexcept>
#include <set>
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <netero/netero.hpp>
#include <netero/graphics/context.hpp>
#include <utils/vkUtils.hpp>

namespace netero::graphics {

    struct Context::impl {
        GLFWwindow* window = nullptr;
    };

    Context::Context(VkInstance vulkanInstance,
        unsigned width,
        unsigned height,
        WindowMode mode,
        const std::string& name, // NOLINT(modernize-pass-by-value)
        const std::string& deviceName)  // NOLINT(modernize-pass-by-value)
        :   _vulkanInstance(vulkanInstance),
            _height(height),
            _width(width),
            _windowMode(mode),
            _name(name),
            _deviceName(deviceName),
            _pImpl(std::make_unique<Context::impl>())
    {
        // Init GLFW Window
        if (_windowMode == WindowMode::FIX) {
            glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
        }
        this->_pImpl->window = glfwCreateWindow(static_cast<int>(this->_width),
            static_cast<int>(this->_height),
            this->_name.c_str(),
            nullptr,
            nullptr);
        const VkResult result = glfwCreateWindowSurface(this->_vulkanInstance,
            this->_pImpl->window,
            nullptr,
            &this->_surface);
        if (result != VK_SUCCESS) {
            throw std::runtime_error("Failed to create window surface.");
        }
        // Pick physical device
        this->_physicalDevice = vkUtils::getBestDevice(this->_vulkanInstance, this->_surface);
        if (!this->_physicalDevice) { throw std::runtime_error("No suitable devices has been found."); }
        if (!vkUtils::checkDeviceSuitable(this->_physicalDevice, vkUtils::defaultDeviceExtensions)) {
            throw std::runtime_error("The device (" + vkUtils::getDeviceName(this->_physicalDevice) + ") is not suitable for windowed context.");
        }
        const auto swapChainDetails = vkUtils::QuerySwapChainSupport(this->_physicalDevice, this->_surface);
        if (swapChainDetails.formats.empty() || swapChainDetails.presentMode.empty()) {
            throw std::runtime_error("The device (" + vkUtils::getDeviceName(this->_physicalDevice) + ") is as no suitable swap chain.");
        }
        this->createLogicalDevice(this->_physicalDevice);
        this->createSwapchain();
        this->createImageViews();
    }

    Context::~Context() {
        for (auto frameBuffer : this->_swapchainFrameBuffers) {
            vkDestroyFramebuffer(this->_logicalDevice, frameBuffer, nullptr);
        }
        vkDestroyPipeline(this->_logicalDevice, this->_graphicsPipeline, nullptr);
        vkDestroyPipelineLayout(this->_logicalDevice, this->_pipelineLayout, nullptr);
        vkDestroyRenderPass(this->_logicalDevice, this->_renderPass, nullptr);
        for (auto imageView: this->_swapchainImageViews) {
            vkDestroyImageView(this->_logicalDevice, imageView, nullptr);
        }
        vkDestroySwapchainKHR(this->_logicalDevice, this->_swapchain, nullptr);
        vkDestroySurfaceKHR(this->_vulkanInstance, this->_surface, nullptr);
        vkDestroyDevice(this->_logicalDevice, nullptr);
        glfwDestroyWindow(this->_pImpl->window);
    }

    void Context::run() {
        this->createRenderPass();
        this->createGraphicsPipeline();
        this->createFrameBuffers();
        while (!glfwWindowShouldClose(this->_pImpl->window)) {
            glfwPollEvents();
        }
    }

    void Context::createLogicalDevice(VkPhysicalDevice device) {
        auto indices = vkUtils::findQueueFamilies(device, this->_surface);
        VkPhysicalDeviceFeatures    deviceFeatures{};
        VkDeviceCreateInfo          deviceCreateInfo{};

        float   queuePriority = 1.f;
        std::vector<VkDeviceQueueCreateInfo>    queueCreateInfos;
        std::set<uint32_t>  uniqueQueueFamilies = { indices.graphicsFamily.value(), indices.presentFamily.value() };
        for (auto queueFamily: uniqueQueueFamilies) {
            VkDeviceQueueCreateInfo queueCreateInfo = {};
            queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
            queueCreateInfo.queueFamilyIndex = queueFamily;
            queueCreateInfo.queueCount = 1;
            queueCreateInfo.pQueuePriorities = &queuePriority;
            queueCreateInfos.push_back(queueCreateInfo);
        }

        deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
        deviceCreateInfo.pQueueCreateInfos = queueCreateInfos.data();
        deviceCreateInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
        deviceCreateInfo.pEnabledFeatures = &deviceFeatures;
        deviceCreateInfo.enabledExtensionCount = static_cast<uint32_t>(vkUtils::defaultDeviceExtensions.size());
        deviceCreateInfo.ppEnabledExtensionNames = vkUtils::defaultDeviceExtensions.data();
        if (netero::isDebugMode) {
            deviceCreateInfo.enabledLayerCount = static_cast<uint32_t>(vkUtils::validationLayers.size());
            deviceCreateInfo.ppEnabledLayerNames = vkUtils::validationLayers.data();
        }
        else {
            deviceCreateInfo.enabledLayerCount = 0;
        }
        const VkResult result = vkCreateDevice(device, &deviceCreateInfo, nullptr, &this->_logicalDevice);
        if (result != VK_SUCCESS) {
            throw std::runtime_error("Failed to create logical device.");
        }
        vkGetDeviceQueue(this->_logicalDevice, indices.graphicsFamily.value(), 0, &this->_graphicsQueue);
        vkGetDeviceQueue(this->_logicalDevice, indices.presentFamily.value(), 0, &this->_presentQueue);
    }

    void Context::createSwapchain() {
        vkUtils::SwapChainSupportDetails    swapChainSupport = vkUtils::QuerySwapChainSupport(this->_physicalDevice, this->_surface);
        vkUtils::QueueFamilyIndices         indices = vkUtils::findQueueFamilies(this->_physicalDevice, this->_surface);
        uint32_t    queueFamilyIndices[] = { indices.graphicsFamily.value(), indices.presentFamily.value() };

        const VkSurfaceFormatKHR surfaceFormat = vkUtils::ChooseSwapSurfaceFormat(swapChainSupport.formats);
        const VkPresentModeKHR presentMode = vkUtils::ChooseSwapPresentMode(swapChainSupport.presentMode);
        const VkExtent2D extent = vkUtils::ChooseSwapExtent(swapChainSupport.capabilities, this->_height, this->_width);
        uint32_t imageCount = swapChainSupport.capabilities.minImageCount + 1;
        if (swapChainSupport.capabilities.maxImageCount > 0 && imageCount > swapChainSupport.capabilities.maxImageCount) {
            imageCount = swapChainSupport.capabilities.maxImageCount;
        }
        VkSwapchainCreateInfoKHR createInfo = {};
        createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
        createInfo.surface = this->_surface;
        createInfo.minImageCount = imageCount;
        createInfo.imageFormat = surfaceFormat.format;
        createInfo.imageColorSpace = surfaceFormat.colorSpace;
        createInfo.imageExtent = extent;
        createInfo.imageArrayLayers = 1;
        createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
        if (indices.graphicsFamily != indices.presentFamily) {
            createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
            createInfo.queueFamilyIndexCount = 2;
            createInfo.pQueueFamilyIndices = queueFamilyIndices;
        }
        else {
            createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
            createInfo.queueFamilyIndexCount = 0;
            createInfo.pQueueFamilyIndices = nullptr;
        }
        createInfo.preTransform = swapChainSupport.capabilities.currentTransform;
        createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
        createInfo.presentMode = presentMode;
        createInfo.clipped = VK_TRUE;
        createInfo.oldSwapchain = nullptr;
        const VkResult result = vkCreateSwapchainKHR(this->_logicalDevice, &createInfo, nullptr, &this->_swapchain);
        if (result != VK_SUCCESS) {
            throw std::runtime_error("Failed to create the swapchain.");
        }
        vkGetSwapchainImagesKHR(this->_logicalDevice, this->_swapchain, &imageCount, nullptr);
        this->_swapchainImage.resize(imageCount);
        vkGetSwapchainImagesKHR(this->_logicalDevice, this->_swapchain, &imageCount, this->_swapchainImage.data());
        this->_swapchainImageFormat = surfaceFormat.format;
        this->_swapchainExtent = extent;
    }

    void Context::createImageViews() {
        VkImageViewCreateInfo   createInfo = {};

        createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
        createInfo.format = this->_swapchainImageFormat;
        createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        createInfo.subresourceRange.baseMipLevel = 0;
        createInfo.subresourceRange.levelCount = 1;
        createInfo.subresourceRange.baseArrayLayer = 0;
        createInfo.subresourceRange.layerCount = 1;

        this->_swapchainImageViews.resize(this->_swapchainImage.size());
        for (unsigned idx = 0; idx < this->_swapchainImageViews.size(); idx++) {
            createInfo.image = this->_swapchainImage[idx];
            const VkResult result = vkCreateImageView(this->_logicalDevice,
                &createInfo,
                nullptr,
                &this->_swapchainImageViews[idx]);
            if (result != VK_SUCCESS) {
                throw std::runtime_error("Failed to create image views.");
            }
        }
    }


}

