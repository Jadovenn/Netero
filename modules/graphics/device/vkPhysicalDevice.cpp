/**
 * Netero sources under BSD-3-Clause
 * see LICENSE.txt
 */

#include <vector>
#include <netero/netero.hpp>
#include <netero/graphics/physicalDevice.hpp>

std::unique_ptr<netero::graphics::PhysicalDevice>   netero::graphics::PhysicalDevice::sInstance = nullptr;

void netero::graphics::PhysicalDevice::Initialize(VkInstance instance) {
    if (unlikely(!sInstance)) {
        PhysicalDevice::sInstance = std::move(std::unique_ptr<PhysicalDevice>(new PhysicalDevice));
        PhysicalDevice::sInstance->_instance = instance;
        vkEnumeratePhysicalDevices(PhysicalDevice::sInstance->_instance,
            &PhysicalDevice::sInstance->_deviceCount,
            nullptr);
        std::vector<VkPhysicalDevice> devices(PhysicalDevice::sInstance->_deviceCount);
        vkEnumeratePhysicalDevices(instance,
            &PhysicalDevice::sInstance->_deviceCount,
            devices.data());
        int maxScore = 0;
        for (const auto& device : devices) {
            const int deviceScore = PhysicalDevice::rateDeviceSuitability(device);
            if (maxScore < deviceScore) {
                maxScore = deviceScore;
                PhysicalDevice::sInstance->_physicalPhysicalDevice = device;
            }
        }
    }
}

std::string netero::graphics::PhysicalDevice::getCurrentDeviceName() const {
    if (!this->_physicalPhysicalDevice) { return ""; }
    VkPhysicalDeviceProperties  deviceProperties;
    vkGetPhysicalDeviceProperties(this->_physicalPhysicalDevice, &deviceProperties);
    return deviceProperties.deviceName;
}

std::multimap<int, std::string> netero::graphics::PhysicalDevice::getDevices() const {
    std::multimap<int, std::string> devicesMap;
    std::vector<VkPhysicalDevice> devices(PhysicalDevice::sInstance->_deviceCount);
    vkEnumeratePhysicalDevices(this->_instance,
        &PhysicalDevice::sInstance->_deviceCount,
        devices.data());
    for (const auto& device: devices) {
        const int deviceScore = PhysicalDevice::rateDeviceSuitability(device);
        VkPhysicalDeviceProperties  deviceProperties;
        vkGetPhysicalDeviceProperties(device, &deviceProperties);
        devicesMap.emplace(deviceScore, deviceProperties.deviceName);
    }
    return devicesMap;
}

netero::graphics::PhysicalDevice* netero::graphics::PhysicalDevice::Instance() {
    return PhysicalDevice::sInstance.get();
}

void netero::graphics::PhysicalDevice::Release() {
    PhysicalDevice::sInstance.reset(nullptr);
}

netero::graphics::PhysicalDevice::~PhysicalDevice() = default;

netero::graphics::QueueFamilyIndices    netero::graphics::PhysicalDevice::findQueueFamilies(VkPhysicalDevice device) {
    QueueFamilyIndices  indices;
    uint32_t            queueFamilyCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);
    std::vector<VkQueueFamilyProperties>    queueFamilies(queueFamilyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());
    int idx = 0;
    for (const auto& queueFamily: queueFamilies) {
        if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
            indices.graphicsFamily = idx;
        }
    }
    return indices;
}

VkPhysicalDevice netero::graphics::PhysicalDevice::getPhysicalDevice() const {
    return this->_physicalPhysicalDevice;
}

int netero::graphics::PhysicalDevice::rateDeviceSuitability(VkPhysicalDevice device) {
    int score = 0;
    if (!device) { return -1; }
    VkPhysicalDeviceProperties  deviceProperties;
    VkPhysicalDeviceFeatures    deviceFeatures;
    vkGetPhysicalDeviceProperties(device, &deviceProperties);
    vkGetPhysicalDeviceFeatures(device, &deviceFeatures);
    const auto queueFamilyIndices = netero::graphics::PhysicalDevice::findQueueFamilies(device);

    if (!deviceFeatures.geometryShader) {
        return -1;
    }
    if (!queueFamilyIndices.isGraphicsSuitable()) {
        return -2;
    }
    switch (deviceProperties.deviceType) {
    case VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU:
        score += 1000;
        break;
    case VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU:
        score += 100;
        break;
    case VK_PHYSICAL_DEVICE_TYPE_CPU:
    case VK_PHYSICAL_DEVICE_TYPE_VIRTUAL_GPU:
        break;
    default:
        return 0;
    }
    score += deviceProperties.limits.maxImageDimension2D;
    score += deviceProperties.limits.maxImageDimension3D;
    return score;
}

