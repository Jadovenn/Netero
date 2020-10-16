/**
* Netero sources under BSD-3-Clause
* see LICENSE.txt
*/

#include <Vulkan/Context/Context.hpp>
#include <Vulkan/VkUtils.hpp>

#include <Netero/Logger.hpp>

namespace VkUtils {

int32_t FindMemoryType(VkPhysicalDevice      physicalDevice,
                       uint32_t              typeFilter,
                       VkMemoryPropertyFlags properties)
{
    VkPhysicalDeviceMemoryProperties memoryProperties;
    vkGetPhysicalDeviceMemoryProperties(physicalDevice, &memoryProperties);
    for (uint32_t idx = 0; idx < memoryProperties.memoryTypeCount; ++idx) {
        if (typeFilter & (1 << idx) &&
            (memoryProperties.memoryTypes[idx].propertyFlags & properties) == properties) {
            return idx;
        }
    }
    return -1;
}

std::pair<VkBuffer, VkDeviceMemory> AllocBuffer(Netero::Gfx::Context& aContext,
                                                VkDeviceSize          size,
                                                VkBufferUsageFlags    usages,
                                                VkMemoryPropertyFlags properties)
{
    std::pair<VkBuffer, VkDeviceMemory> memory { nullptr, nullptr };

    VkBufferCreateInfo bufferInfo {};
    bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferInfo.size = size;
    bufferInfo.usage = usages;
    bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    if (vkCreateBuffer(aContext.myLogicalDevice, &bufferInfo, nullptr, &memory.first) !=
        VK_SUCCESS) {
        throw std::runtime_error("GPU alloc failed.");
    }

    VkMemoryRequirements memRequirements;
    vkGetBufferMemoryRequirements(aContext.myLogicalDevice, memory.first, &memRequirements);

    VkMemoryAllocateInfo allocInfo {};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = memRequirements.size;
    allocInfo.memoryTypeIndex =
        FindMemoryType(aContext.myPhysicalDevice, memRequirements.memoryTypeBits, properties);

    if (vkAllocateMemory(aContext.myLogicalDevice, &allocInfo, nullptr, &memory.second) !=
        VK_SUCCESS) {
        LOG_ERROR << "Failed to allocate buffer memory!" << std::endl;
        return { nullptr, nullptr };
    }

    vkBindBufferMemory(aContext.myLogicalDevice, memory.first, memory.second, 0);
    return memory;
}

std::pair<VkImage, VkDeviceMemory> AllocImage(Netero::Gfx::Context& aContext,
                                              uint32_t              width,
                                              uint32_t              height,
                                              VkFormat              format,
                                              VkImageTiling         tiling,
                                              VkImageUsageFlags     usages,
                                              VkMemoryPropertyFlags properties)
{
    VkImage           image = nullptr;
    VkDeviceMemory    memory = nullptr;
    VkImageCreateInfo createInfo {};
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
    VkResult result = vkCreateImage(aContext.myLogicalDevice, &createInfo, nullptr, &image);
    if (result != VK_SUCCESS) {
        throw std::runtime_error("Could not allocate image.");
    }
    VkMemoryRequirements memRequirements;
    vkGetImageMemoryRequirements(aContext.myLogicalDevice, image, &memRequirements);
    VkMemoryAllocateInfo allocInfo {};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = memRequirements.size;
    allocInfo.memoryTypeIndex =
        FindMemoryType(aContext.myPhysicalDevice, memRequirements.memoryTypeBits, properties);
    result = vkAllocateMemory(aContext.myLogicalDevice, &allocInfo, nullptr, &memory);
    if (result != VK_SUCCESS) {
        LOG_ERROR << "Could not allocate image memory!" << std::endl;
        return { nullptr, nullptr };
    }
    vkBindImageMemory(aContext.myLogicalDevice, image, memory, 0);
    return { image, memory };
}
} // namespace VkUtils
