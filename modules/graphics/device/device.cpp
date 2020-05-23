/**
 * Netero sources under BSD-3-Clause
 * see LICENSE.txt
 */

#include <set>
#include <stdexcept>
#include <netero/netero.hpp>
#include <netero/graphics/device.hpp>

#include "utils/vkUtils.hpp"

namespace netero::graphics {

    Device::Device(VkInstance instance, VkSurfaceKHR surface)
        :   _instance(instance),
            _surface(surface),
            physicalDevice(nullptr),
            logicalDevice(nullptr),
            graphicsQueue(nullptr),
            presentQueue(nullptr)
    {
        this->pickPhysicalDevice();
        this->createLogicalDevice(this->physicalDevice);
        this->deviceName = vkUtils::getDeviceName(this->physicalDevice);
    }

    void Device::pickPhysicalDevice() {
        // Pick physical device
        this->physicalDevice = vkUtils::getBestDevice(this->_instance, this->_surface);
        if (!this->physicalDevice) { throw std::runtime_error("No suitable devices has been found."); }
        if (!vkUtils::checkDeviceSuitable(this->physicalDevice, vkUtils::defaultDeviceExtensions)) {
            throw std::runtime_error("The device (" + vkUtils::getDeviceName(this->physicalDevice) + ") is not suitable for windowed context.");
        }
        const auto swapChainDetails = vkUtils::QuerySwapChainSupport(this->physicalDevice, this->_surface);
        if (swapChainDetails.formats.empty() || swapChainDetails.presentMode.empty()) {
            throw std::runtime_error("The device (" + vkUtils::getDeviceName(this->physicalDevice) + ") is as no suitable swap chain.");
        }
    }

    void Device::createLogicalDevice(VkPhysicalDevice device) {
        auto indices = vkUtils::findQueueFamilies(device, this->_surface);
        VkPhysicalDeviceFeatures    deviceFeatures{};
        VkDeviceCreateInfo          deviceCreateInfo{};

        float   queuePriority = 1.f;
        std::vector<VkDeviceQueueCreateInfo>    queueCreateInfos;
        std::set<uint32_t>  uniqueQueueFamilies = { indices.graphicsFamily.value(), indices.presentFamily.value() };
        for (auto queueFamily : uniqueQueueFamilies) {
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
        // ReSharper disable once CppUnreachableCode
        else {
            deviceCreateInfo.enabledLayerCount = 0;
        }
        const VkResult result = vkCreateDevice(device, &deviceCreateInfo, nullptr, &this->logicalDevice);
        if (result != VK_SUCCESS) {
            throw std::runtime_error("Failed to create logical device.");
        }
        vkGetDeviceQueue(this->logicalDevice, indices.graphicsFamily.value(), 0, &this->graphicsQueue);
        vkGetDeviceQueue(this->logicalDevice, indices.presentFamily.value(), 0, &this->presentQueue);
    }

}

