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

    void        TransferBuffer(netero::graphics::Device* device, VkBuffer source, VkBuffer destination, VkDeviceSize size) {
        VkCommandBufferAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        allocInfo.commandPool = device->transferCommandPool;
        allocInfo.commandBufferCount = 1;

        VkCommandBuffer commandBuffer;
        vkAllocateCommandBuffers(device->logicalDevice, &allocInfo, &commandBuffer);
        VkCommandBufferBeginInfo beginInfo{};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

        vkBeginCommandBuffer(commandBuffer, &beginInfo);
        VkBufferCopy copyRegion{};
        copyRegion.srcOffset = 0;
        copyRegion.dstOffset = 0;
        copyRegion.size = size;
        vkCmdCopyBuffer(commandBuffer, source, destination, 1, &copyRegion);
        vkEndCommandBuffer(commandBuffer);
        VkSubmitInfo submitInfo{};
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = &commandBuffer;

        vkQueueSubmit(device->transferQueue, 1, &submitInfo, nullptr);
        vkQueueWaitIdle(device->transferQueue);
        vkFreeCommandBuffers(device->logicalDevice, device->transferCommandPool, 1, &commandBuffer);
    }

    std::pair<VkImage, VkDeviceMemory>  AllocImage(netero::graphics::Device* device, uint32_t width, uint32_t height, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usages, VkMemoryPropertyFlags properties) {
        VkImage image = nullptr;
        VkDeviceMemory memory = nullptr;
        VkImageCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
        createInfo.imageType = VK_IMAGE_TYPE_2D;
        createInfo.extent.width = width;
        createInfo.extent.height = height;
        createInfo.extent.depth = 1;
        createInfo.mipLevels = 1;
        createInfo.arrayLayers = 1;
        createInfo.format = format;
        createInfo.tiling = tiling;
        createInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        createInfo.usage = usages;
        createInfo.samples = VK_SAMPLE_COUNT_1_BIT;
        createInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
        VkResult result = vkCreateImage(device->logicalDevice, &createInfo, nullptr, &image);
        if (result != VK_SUCCESS) {
            throw std::runtime_error("Could not allocate image.");
        }
        VkMemoryRequirements    memRequirements;
        vkGetImageMemoryRequirements(device->logicalDevice, image, &memRequirements);
        VkMemoryAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        allocInfo.allocationSize = memRequirements.size;
        allocInfo.memoryTypeIndex = FindMemoryType(device->physicalDevice, memRequirements.memoryTypeBits, properties);
        result = vkAllocateMemory(device->logicalDevice, &allocInfo, nullptr, &memory);
        if (result != VK_SUCCESS) {
            throw std::runtime_error("Could not allocate image memory.");
        }
        vkBindImageMemory(device->logicalDevice, image, memory, 0);
        return { image, memory };
    }

}

