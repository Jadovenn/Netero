/**
 * Netero sources under BSD-3-Clause
 * see LICENSE.txt
 */

#include <stdexcept>
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <netero/graphics/context.hpp>
#include <netero/graphics/physicalDevice.hpp>
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
        this->_pImpl->window = glfwCreateWindow(this->_width,
            this->_height,
            this->_name.c_str(),
            nullptr,
            nullptr);
        this->_physicalDevice = PhysicalDevice::Instance()->getPhysicalDevice();
        this->setPhysicalDevice(this->_physicalDevice);
        const VkResult result = glfwCreateWindowSurface(this->_vulkanInstance,
            this->_pImpl->window,
            nullptr,
            &this->_surface);
        if (result != VK_SUCCESS) {
            throw std::runtime_error("Failed to create window surface.");
        }
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
        auto indices = netero::graphics::PhysicalDevice::findQueueFamilies(device);
        VkDeviceQueueCreateInfo     queueCreateInfo{};
        VkPhysicalDeviceFeatures    deviceFeatures{};
        VkDeviceCreateInfo          deviceCreateInfo{};

        float   queuePriority = 1.f;
        queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queueCreateInfo.queueFamilyIndex = indices.graphicsFamily.value();
        queueCreateInfo.queueCount = 1;
        queueCreateInfo.pQueuePriorities = &queuePriority;

        deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
        deviceCreateInfo.pQueueCreateInfos = &queueCreateInfo;
        deviceCreateInfo.queueCreateInfoCount = 1;
        deviceCreateInfo.pEnabledFeatures = &deviceFeatures;
        deviceCreateInfo.enabledExtensionCount = 0;
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
    }

}

