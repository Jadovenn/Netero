/**
 * Netero sources under BSD-3-Clause
 * see LICENSE.txt
 */

#include "utils/vkUtils.hpp"

namespace vkUtils {

    int32_t    FindMemoryType(VkPhysicalDevice physicalDevice, uint32_t typeFilter, VkMemoryPropertyFlags properties) {
        VkPhysicalDeviceMemoryProperties memoryProperties;
        vkGetPhysicalDeviceMemoryProperties(physicalDevice, &memoryProperties);
        for (uint32_t idx = 0; idx < memoryProperties.memoryTypeCount; ++idx) {
            if (typeFilter & (1 << idx) && (memoryProperties.memoryTypes[idx].propertyFlags & properties) == properties) {
                return idx;
            }
        }
        return -1;
    }
}

