/**
 * Netero sources under BSD-3-Clause
 * see LICENSE.txt
 */

#include "Vulkan/VkUtils.hpp"

namespace VkUtils {

VkCommandBuffer BeginCommandBufferRecording(VkDevice                  aDevice,
                                            VkCommandPool             aCommandPool,
                                            VkCommandBufferUsageFlags someFlags)
{
    VkCommandBufferAllocateInfo allocInfo {};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandPool = aCommandPool;
    allocInfo.commandBufferCount = 1;

    VkCommandBuffer commandBuffer;
    vkAllocateCommandBuffers(aDevice, &allocInfo, &commandBuffer);

    VkCommandBufferBeginInfo beginInfo {};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = someFlags;

    vkBeginCommandBuffer(commandBuffer, &beginInfo);
    return commandBuffer;
}
void            EndCommandBuffer(VkCommandBuffer aCommandBuffer) {
    vkEndCommandBuffer(aCommandBuffer);
}

void            FlushCommandBuffer(VkDevice aDevice, VkQueue aQueue, VkCommandPool aCommandPool, VkCommandBuffer aCommandBuffer) {
    VkSubmitInfo submitInfo {};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &aCommandBuffer;

    vkQueueSubmit(aQueue, 1, &submitInfo, nullptr);
    vkQueueWaitIdle(aQueue);
    vkFreeCommandBuffers(aDevice, aCommandPool, 1, &aCommandBuffer);
}

} // namespace VkUtils
