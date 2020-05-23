/**
 * Netero sources under BSD-3-Clause
 * see LICENSE.txt
 */

#include <stdexcept>
#include <netero/graphics/device.hpp>
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

    std::pair<VkBuffer, VkDeviceMemory>    AllocBuffer(netero::graphics::Device* device, VkDeviceSize size, VkBufferUsageFlags usages, VkMemoryPropertyFlags properties) {
        std::pair<VkBuffer, VkDeviceMemory> memory{nullptr, nullptr};

        VkBufferCreateInfo bufferInfo{};
        bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        bufferInfo.size = size;
        bufferInfo.usage = usages;
        bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

        if (vkCreateBuffer(device->logicalDevice, &bufferInfo, nullptr, &memory.first) != VK_SUCCESS) {
            throw std::runtime_error("GPU alloc failed.");
        }

        VkMemoryRequirements memRequirements;
        vkGetBufferMemoryRequirements(device->logicalDevice, memory.first, &memRequirements);

        VkMemoryAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        allocInfo.allocationSize = memRequirements.size;
        allocInfo.memoryTypeIndex = FindMemoryType(device->physicalDevice, memRequirements.memoryTypeBits, properties);

        if (vkAllocateMemory(device->logicalDevice, &allocInfo, nullptr, &memory.second) != VK_SUCCESS) {
            throw std::runtime_error("failed to allocate buffer memory!");
        }

        vkBindBufferMemory(device->logicalDevice, memory.first, memory.second, 0);
        return memory;
    }

}

