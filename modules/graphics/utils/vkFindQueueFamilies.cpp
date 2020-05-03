/**
 * Netero sources under BSD-3-Clause
 * see LICENSE.txt
 */

#include <set>
#include "utils/vkUtils.hpp"

const std::vector<const char*> vkUtils::defaultDeviceExtensions = {
    VK_KHR_SWAPCHAIN_EXTENSION_NAME
};

vkUtils::QueueFamilyIndices    vkUtils::findQueueFamilies(VkPhysicalDevice device, VkSurfaceKHR surface) {
    QueueFamilyIndices  indices;
    uint32_t            queueFamilyCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);
    std::vector<VkQueueFamilyProperties>    queueFamilies(queueFamilyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());
    int idx = 0;
    for (const auto& queueFamily : queueFamilies) {
        if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
            indices.graphicsFamily = idx;
        }
        VkBool32 presentSupport = false;
        vkGetPhysicalDeviceSurfaceSupportKHR(device, idx, surface, &presentSupport);
        if (presentSupport) {
            indices.presentFamily = idx;
        }
    }
    return indices;
}

bool    vkUtils::checkDeviceSuitable(VkPhysicalDevice device, const std::vector<const char*> extensions) {
    uint32_t    extensionCount = 0;
    vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);
    std::vector<VkExtensionProperties>  availableExtension(extensionCount);
    vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, availableExtension.data());
    std::set<std::string>   requiredExtensions(extensions.begin(), extensions.end());
    for (const auto& ext: availableExtension) {
        requiredExtensions.erase(ext.extensionName);
    }
    return requiredExtensions.empty();
}

int vkUtils::rateDeviceSuitability(VkPhysicalDevice device, VkSurfaceKHR surface) {
    int score = 0;
    if (!device) { return -1; }
    VkPhysicalDeviceProperties  deviceProperties;
    VkPhysicalDeviceFeatures    deviceFeatures;
    vkGetPhysicalDeviceProperties(device, &deviceProperties);
    vkGetPhysicalDeviceFeatures(device, &deviceFeatures);
    const auto queueFamilyIndices = vkUtils::findQueueFamilies(device, surface);

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

std::multimap<int, VkPhysicalDevice>    vkUtils::getRatedAvailableDevices(VkInstance instance, VkSurfaceKHR surface) {
    std::multimap<int, VkPhysicalDevice>    devicesMap;
    uint32_t                                deviceCount;
    vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);
    std::vector<VkPhysicalDevice> devices(deviceCount);
    vkEnumeratePhysicalDevices(instance, &deviceCount, devices.data());
    for (const auto& device : devices) {
        const int deviceScore = vkUtils::rateDeviceSuitability(device, surface);
        devicesMap.emplace(deviceScore, device);
    }
    return devicesMap;
}

VkPhysicalDevice    vkUtils::getBestDevice(VkInstance instance, VkSurfaceKHR surface) {
    uint32_t            deviceCount = 0;
    VkPhysicalDevice    physicalDevice = nullptr;
    int                 bestScore = -1;

    vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);
    std::vector<VkPhysicalDevice> devices(deviceCount);
    vkEnumeratePhysicalDevices(instance, &deviceCount, devices.data());
    for (const auto& device : devices) {
        const int deviceScore = vkUtils::rateDeviceSuitability(device, surface);
        if (deviceScore > bestScore) {
            bestScore = deviceScore;
            physicalDevice = device;
        }
    }
    return physicalDevice;
}

std::string vkUtils::getDeviceName(VkPhysicalDevice device) {
    if (!device) { return ""; }
    VkPhysicalDeviceProperties  deviceProperties;
    vkGetPhysicalDeviceProperties(device, &deviceProperties);
    return deviceProperties.deviceName;
}

VkPhysicalDevice vkUtils::getDeviceByName(const char* name, VkInstance instance) {
    uint32_t    deviceCount = 0;
    vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);
    std::vector<VkPhysicalDevice> devices(deviceCount);
    vkEnumeratePhysicalDevices(instance, &deviceCount, devices.data());
    for (auto *device: devices) {
        VkPhysicalDeviceProperties  property;
        vkGetPhysicalDeviceProperties(device, &property);
        if (strstr(name, property.deviceName)) {
            return device;
        }
    }
    return nullptr;
}

std::vector<std::string> vkUtils::getDevicesName(VkInstance instance) {
    uint32_t    deviceCount = 0;
    vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);
    std::vector<VkPhysicalDevice>   devices(deviceCount);
    std::vector<std::string>        devicesName(deviceCount);
    vkEnumeratePhysicalDevices(instance, &deviceCount, devices.data());
    for (auto *device: devices) {
        VkPhysicalDeviceProperties  property;
        vkGetPhysicalDeviceProperties(device, &property);
        devicesName.emplace_back(property.deviceName);
    }
    return devicesName;
}


