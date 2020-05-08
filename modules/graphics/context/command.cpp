/**
 * Netero sources under BSD-3-Clause
 * see LICENSE.txt
 */

#include <stdexcept>
#include "utils/vkUtils.hpp"
#include <netero/graphics/context.hpp>

namespace netero::graphics {
    void Context::createCommandPool() {
        vkUtils::QueueFamilyIndices queueFamilyIndices = vkUtils::findQueueFamilies(this->_physicalDevice, this->_surface);

        VkCommandPoolCreateInfo poolInfo{};
        poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
        poolInfo.queueFamilyIndex = queueFamilyIndices.graphicsFamily.value();
        poolInfo.flags = 0;
        if (vkCreateCommandPool(this->_logicalDevice, &poolInfo, nullptr, &this->_commandPool) != VK_SUCCESS) {
            throw std::runtime_error("Failed to create command pool.");
        }
    }

    void Context::createCommandBuffers() {
        this->_commandBuffers.resize(this->_swapchainFrameBuffers.size());
        VkCommandBufferAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocInfo.commandPool = this->_commandPool;
        allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        allocInfo.commandBufferCount = static_cast<uint32_t>(this->_commandBuffers.size());

        if (vkAllocateCommandBuffers(this->_logicalDevice, &allocInfo, this->_commandBuffers.data()) != VK_SUCCESS) {
            throw std::runtime_error("Failed to allocate command buffers.");
        }
        for (size_t i = 0; i < this->_commandBuffers.size(); i++) {
            VkCommandBufferBeginInfo beginInfo{};
            beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
            beginInfo.flags = 0;
            beginInfo.pInheritanceInfo = nullptr;

            if (vkBeginCommandBuffer(_commandBuffers[i], &beginInfo) != VK_SUCCESS) {
                throw std::runtime_error("Failed to begin recording command buffer.");
            }
            VkClearValue clearColor = { 0.0f, 0.0f, 0.0f, 1.0f };
            VkRenderPassBeginInfo renderPassInfo{};
            renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
            renderPassInfo.renderPass = this->_renderPass;
            renderPassInfo.framebuffer = this->_swapchainFrameBuffers[i];
            renderPassInfo.renderArea.offset = { 0, 0 };
            renderPassInfo.renderArea.extent = this->_swapchainExtent;
            renderPassInfo.clearValueCount = 1;
            renderPassInfo.pClearValues = &clearColor;
            vkCmdBeginRenderPass(this->_commandBuffers[i], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
            vkCmdBindPipeline(this->_commandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, this->_graphicsPipeline);
            vkCmdDraw(this->_commandBuffers[i], 3, 1, 0, 0);
            vkCmdEndRenderPass(this->_commandBuffers[i]);
            if (vkEndCommandBuffer(this->_commandBuffers[i]) != VK_SUCCESS) {
                throw std::runtime_error("Failed to record command buffer.");
            }
        }
    }

    void Context::createSemaphores() {
        VkSemaphoreCreateInfo semaphoreInfo{};
        semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
        VkResult result = vkCreateSemaphore(this->_logicalDevice,
            &semaphoreInfo,
            nullptr,
            &this->_imageAvailableSemaphore);
        if (result != VK_SUCCESS) {
            throw std::runtime_error("Failed to create imageAvailable semaphore.");
        }
        result = vkCreateSemaphore(this->_logicalDevice,
            &semaphoreInfo,
            nullptr,
            &this->_renderFinishedSemaphore);
        if (result != VK_SUCCESS) {
            throw std::runtime_error("Failed to create renderFinished semaphore.");
        }
    }


}

