/**
 * Netero sources under BSD-3-Clause
 * see LICENSE.txt
 */

#include <stdexcept>

#include <netero/graphics/device.hpp>

#include "utils/vkUtils.hpp"

namespace vkUtils {

void TransferBuffer(netero::graphics::Device* device,
                    VkBuffer                  source,
                    VkBuffer                  destination,
                    VkDeviceSize              size)
{
    VkCommandBuffer cmdBuffer =
        BeginCommandBufferRecording(device->logicalDevice,
                                    device->transferCommandPool,
                                    VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);

    VkBufferCopy copyRegion {};
    copyRegion.srcOffset = 0;
    copyRegion.dstOffset = 0;
    copyRegion.size = size;
    vkCmdCopyBuffer(cmdBuffer, source, destination, 1, &copyRegion);

    FlushCommandBuffer(device->logicalDevice,
                       device->transferQueue,
                       device->transferCommandPool,
                       cmdBuffer);
}

void TransferImage(VkCommandBuffer cmdBuffer,
                   VkBuffer        source,
                   VkImage         destination,
                   uint32_t        width,
                   uint32_t        height)
{
    VkBufferImageCopy region {};
    region.bufferOffset = 0;
    region.bufferRowLength = 0;
    region.bufferImageHeight = 0;

    region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    region.imageSubresource.mipLevel = 0;
    region.imageSubresource.baseArrayLayer = 0;
    region.imageSubresource.layerCount = 1;

    region.imageOffset = { 0, 0, 0 };
    region.imageExtent = { width, height, 1 };
    vkCmdCopyBufferToImage(cmdBuffer,
                           source,
                           destination,
                           VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                           1,
                           &region);
}

void TransitionImageLayout(VkCommandBuffer cmdBuffer,
                           VkImage         image,
                           VkFormat        format,
                           VkImageLayout   oldLayout,
                           VkImageLayout   newLayout)
{
    VkImageMemoryBarrier barrier {};
    barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    barrier.oldLayout = oldLayout;
    barrier.newLayout = newLayout;
    barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.image = image;
    barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    barrier.subresourceRange.baseMipLevel = 0;
    barrier.subresourceRange.levelCount = 1;
    barrier.subresourceRange.baseArrayLayer = 0;
    barrier.subresourceRange.layerCount = 1;
    barrier.srcAccessMask = 0; // TODO
    barrier.dstAccessMask = 0; // TODO

    VkPipelineStageFlags sourceStage;
    VkPipelineStageFlags destinationStage;

    if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED &&
        newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) {
        barrier.srcAccessMask = 0;
        barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

        sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
        destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
    }
    else if (oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL &&
             newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) {
        barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

        sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
        destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
    }
    else {
        throw std::invalid_argument("unsupported layout transition!");
    }

    vkCmdPipelineBarrier(cmdBuffer,
                         sourceStage,
                         destinationStage,
                         0,
                         0,
                         nullptr,
                         0,
                         nullptr,
                         1,
                         &barrier);
}

VkImageView CreateImageView(VkDevice device, VkImage image, VkFormat format)
{
    VkImageView           imageView;
    VkImageViewCreateInfo viewInfo {};
    viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    viewInfo.image = image;
    viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
    viewInfo.format = format;
    viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    viewInfo.subresourceRange.baseMipLevel = 0;
    viewInfo.subresourceRange.levelCount = 1;
    viewInfo.subresourceRange.baseArrayLayer = 0;
    viewInfo.subresourceRange.layerCount = 1;
    if (vkCreateImageView(device, &viewInfo, nullptr, &imageView) != VK_SUCCESS) {
        throw std::runtime_error("failed to create texture image view!");
    }
    return imageView;
}

} // namespace vkUtils
