/**
 * Netero sources under BSD-3-Clause
 * see LICENSE.txt
 */

#include <cassert>
#include <set>
#include <stdexcept>

#include <netero/graphics/device.hpp>
#include <netero/netero.hpp>

#include "utils/vkUtils.hpp"

namespace netero::graphics {

Device::Device(VkInstance instance, VkSurfaceKHR surface)
    : _instance(instance),
      _surface(surface),
      _width(0),
      _height(0),
      physicalDevice(nullptr),
      logicalDevice(nullptr),
      graphicsQueue(nullptr),
      presentQueue(nullptr),
      transferQueue(nullptr),
      transferCommandPool(nullptr)
{
    assert(_instance);
    assert(_surface);
    this->pickPhysicalDevice();
    this->createLogicalDevice(this->physicalDevice);
    this->createTransferCommandPool();
    this->deviceName = vkUtils::getDeviceName(this->physicalDevice);
}

Device::~Device()
{
    vkDestroyCommandPool(this->logicalDevice, this->transferCommandPool, nullptr);
    vkDestroyDevice(logicalDevice, nullptr);
}

void Device::pickPhysicalDevice()
{
    // Pick physical device
    this->physicalDevice = vkUtils::getBestDevice(this->_instance, this->_surface);
    if (!this->physicalDevice) {
        throw std::runtime_error("No suitable devices has been found.");
    }
    if (!vkUtils::checkDeviceSuitable(this->physicalDevice, vkUtils::defaultDeviceExtensions)) {
        throw std::runtime_error("The device (" + vkUtils::getDeviceName(this->physicalDevice) +
                                 ") is not suitable for windowed context.");
    }
    const auto swapChainDetails =
        vkUtils::QuerySwapChainSupport(this->physicalDevice, this->_surface);
    if (swapChainDetails.formats.empty() || swapChainDetails.presentMode.empty()) {
        throw std::runtime_error("The device (" + vkUtils::getDeviceName(this->physicalDevice) +
                                 ") is as no suitable swap chain.");
    }
}

VkSurfaceKHR Device::getAssociatedSurface() const
{
    return this->_surface;
}

int Device::getSurfaceHeight() const
{
    return this->_height;
}

void Device::setSurfaceHeight(const int height)
{
    this->_height = height;
}

int Device::getSurfaceWidth() const
{
    return this->_width;
}

void Device::setSurfaceWidth(const int width)
{
    this->_width = width;
}

void Device::createLogicalDevice(VkPhysicalDevice device)
{
    auto                     indices = vkUtils::findQueueFamilies(device, this->_surface);
    VkPhysicalDeviceFeatures deviceFeatures {};
    deviceFeatures.samplerAnisotropy = VK_TRUE;
    VkDeviceCreateInfo deviceCreateInfo {};

    float                                queuePriority = 1.f;
    std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
    std::set<uint32_t>                   uniqueQueueFamilies = { indices.graphicsFamily.value(),
                                               indices.presentFamily.value() };
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
    deviceCreateInfo.enabledExtensionCount =
        static_cast<uint32_t>(vkUtils::defaultDeviceExtensions.size());
    deviceCreateInfo.ppEnabledExtensionNames = vkUtils::defaultDeviceExtensions.data();
    if (netero::isDebugMode) {
        deviceCreateInfo.enabledLayerCount =
            static_cast<uint32_t>(vkUtils::validationLayers.size());
        deviceCreateInfo.ppEnabledLayerNames = vkUtils::validationLayers.data();
    }
    // ReSharper disable once CppUnreachableCode
    else {
        deviceCreateInfo.enabledLayerCount = 0;
    }
    const VkResult result =
        vkCreateDevice(device, &deviceCreateInfo, nullptr, &this->logicalDevice);
    if (result != VK_SUCCESS) {
        throw std::runtime_error("Failed to create logical device.");
    }
    vkGetDeviceQueue(this->logicalDevice, indices.graphicsFamily.value(), 0, &this->graphicsQueue);
    vkGetDeviceQueue(this->logicalDevice, indices.presentFamily.value(), 0, &this->presentQueue);
    if (indices.transferFamily.has_value()) {
        vkGetDeviceQueue(this->logicalDevice,
                         indices.transferFamily.value(),
                         0,
                         &this->transferQueue);
    }
    else {
        this->transferQueue = this->presentQueue;
    }
}

void Device::createTransferCommandPool()
{
    vkUtils::QueueFamilyIndices queueFamilyIndices =
        vkUtils::findQueueFamilies(this->physicalDevice, this->_surface);

    VkCommandPoolCreateInfo poolInfo {};
    poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    if (queueFamilyIndices.transferFamily.has_value()) {
        poolInfo.queueFamilyIndex = queueFamilyIndices.transferFamily.value();
    }
    else {
        poolInfo.queueFamilyIndex = queueFamilyIndices.graphicsFamily.value();
    }
    poolInfo.flags = VK_COMMAND_POOL_CREATE_TRANSIENT_BIT;
    if (vkCreateCommandPool(this->logicalDevice, &poolInfo, nullptr, &this->transferCommandPool) !=
        VK_SUCCESS) {
        throw std::runtime_error("Failed to create transfer command pool.");
    }
}

} // namespace netero::graphics
