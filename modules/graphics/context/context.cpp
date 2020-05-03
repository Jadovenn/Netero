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
        this->_physicalDevice = vkUtils::getBestDevice(this->_vulkanInstance, this->_surface);
        if (!vkUtils::checkDeviceSuitable(this->_physicalDevice, vkUtils::defaultDeviceExtensions)) {
            throw std::runtime_error("The device (" + vkUtils::getDeviceName(this->_physicalDevice) + ") is not suitable for windowed context.");
        }
        const auto swapChainDetails = vkUtils::QuerySwapChainSupport(this->_physicalDevice, this->_surface);
        if (swapChainDetails.formats.empty() || swapChainDetails.presentMode.empty()) {
            throw std::runtime_error("The device (" + vkUtils::getDeviceName(this->_physicalDevice) + ") is as no suitable swap chain.");
        }
        this->setPhysicalDevice(this->_physicalDevice);
    }

    Context::~Context() {
        vkDestroySurfaceKHR(this->_vulkanInstance, this->_surface, nullptr);
        vkDestroyDevice(this->_logicalDevice, nullptr);
        glfwDestroyWindow(this->_pImpl->window);
    }

    void Context::run() const {
        while (!glfwWindowShouldClose(this->_pImpl->window)) {
            glfwPollEvents();
        }
    }

    void Context::setPhysicalDevice(VkPhysicalDevice device) {
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

}

