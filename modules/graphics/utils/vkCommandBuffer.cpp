/**
 * Netero sources under BSD-3-Clause
 * see LICENSE.txt
 */


#include <utils/vkUtils.hpp>

namespace vkUtils {

    VkCommandBuffer    BeginCommandBufferRecording(VkDevice device, VkCommandPool cmdPool, VkCommandBufferUsageFlags flags) {
        VkCommandBufferAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        allocInfo.commandPool = cmdPool;
        allocInfo.commandBufferCount = 1;

        VkCommandBuffer commandBuffer;
        vkAllocateCommandBuffers(device, &allocInfo, &commandBuffer);

        VkCommandBufferBeginInfo beginInfo{};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        beginInfo.flags = flags;

        vkBeginCommandBuffer(commandBuffer, &beginInfo);
        return commandBuffer;
    }

    void    TerminateCommandBuffer(VkCommandBuffer cmdBuffer) {
        vkEndCommandBuffer(cmdBuffer);
    }

    void    FlushCommandBuffer(VkDevice device, VkQueue queue, VkCommandPool cmdPool, VkCommandBuffer cmdBuffer) {
        vkEndCommandBuffer(cmdBuffer);
        VkSubmitInfo    submitInfo{};
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = &cmdBuffer;

        vkQueueSubmit(queue, 1, &submitInfo, nullptr);
        vkQueueWaitIdle(queue);
        vkFreeCommandBuffers(device, cmdPool, 1, &cmdBuffer);
    }

}

