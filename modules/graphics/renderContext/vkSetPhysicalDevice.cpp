/**
 * Netero sources under BSD-3-Clause
 * see LICENSE.txt
 */

#include <stdexcept>
#include <netero/graphics/physicalDevice.hpp>
#include "renderContext/vkRenderContext.hpp"

void netero::graphics::RenderContext::setPhysicalDevice(VkPhysicalDevice device) const {
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
    if (this->pImpl->enableValidationLayers) {
        deviceCreateInfo.enabledLayerCount = static_cast<uint32_t>(this->pImpl->validationLayers.size());
        deviceCreateInfo.ppEnabledLayerNames = this->pImpl->validationLayers.data();
    }
    else {
        deviceCreateInfo.enabledLayerCount = 0;
    }
    const VkResult result = vkCreateDevice(device, &deviceCreateInfo, nullptr, &this->pImpl->logicalDevice);
    if (result != VK_SUCCESS) {
        throw std::runtime_error("Failed to create logical device.");
    }
    vkGetDeviceQueue(this->pImpl->logicalDevice, indices.graphicsFamily.value(), 0, &this->pImpl->graphicsQueue);
}

