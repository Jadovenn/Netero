/**
 * Netero sources under BSD-3-Clause
 * see LICENSE.txt
 */

#include <cassert>
#include <stdexcept>
#include <netero/graphics/model.hpp>
#include <netero/graphics/pipeline.hpp>

#include "utils/vkUtils.hpp"

namespace netero::graphics {

    Pipeline::Pipeline(VkInstance instance, Device* device)
        :   _instance(instance),
            _device(device),
            _renderPass(nullptr),
            _pipelineLayout(nullptr),
            _graphicsPipeline(nullptr),
            _commandPool(nullptr),
            _descriptorPool(nullptr),
            _descriptorSetLayout(nullptr),
            swapchain(nullptr)
    {
        assert(_instance);
        assert(_device);
    }

    Pipeline::~Pipeline() {
        this->release();
        vkDestroyDescriptorSetLayout(this->_device->logicalDevice, this->_descriptorSetLayout, nullptr);
        vkDestroyCommandPool(this->_device->logicalDevice, this->_commandPool, nullptr);

    }

    void Pipeline::build(std::vector<Model*>& models) {
        this->createSwapchain();
        this->createUniformBuffers();
        this->createDescriptorPool();
        this->createDescriptorSetLayout();
        this->createDescriptorSets();
        this->createImageViews();
        this->createRenderPass();
        this->buildModels(models);
        this->createFrameBuffers();
        this->createCommandPool();
        this->createCommandBuffers(models);
    }

    void Pipeline::release() {
        for (auto* frameBuffer : this->_swapchainFrameBuffers) {
            vkDestroyFramebuffer(this->_device->logicalDevice, frameBuffer, nullptr);
        }
        vkFreeCommandBuffers(this->_device->logicalDevice, this->_commandPool, static_cast<uint32_t>(this->commandBuffers.size()), this->commandBuffers.data());
        vkDestroyRenderPass(this->_device->logicalDevice, this->_renderPass, nullptr);
        for (auto* imageView : this->_swapchainImageViews) {
            vkDestroyImageView(this->_device->logicalDevice, imageView, nullptr);
        }
        vkDestroySwapchainKHR(this->_device->logicalDevice, this->swapchain, nullptr);
        for (size_t idx = 0; idx < this->swapchainImages.size(); ++idx) {
            vkDestroyBuffer(this->_device->logicalDevice, _uniformBuffers[idx], nullptr);
            vkFreeMemory(this->_device->logicalDevice, _uniformBuffersMemory[idx], nullptr);
        }
        vkDestroyDescriptorPool(this->_device->logicalDevice, this->_descriptorPool, nullptr);
    }

    void Pipeline::rebuild(std::vector<Model*>& models) {
        this->release();
        this->createSwapchain();
        this->createUniformBuffers();
        this->createDescriptorPool();
        this->createDescriptorSets();
        this->createImageViews();
        this->createRenderPass();
        this->rebuildModels(models);
        this->createFrameBuffers();
        this->createCommandBuffers(models);
    }

    void Pipeline::buildModels(std::vector<Model*>& models) const {
        const size_t swapchainImagesCount = this->swapchainImages.size();
        for (auto* model : models) {
            model->build(swapchainImagesCount, this->_renderPass, this->_descriptorSetLayout, this->swapchainExtent);
        }
    }

    void Pipeline::rebuildModels(std::vector<Model*>& models) const {
        const size_t swapchainImagesCount = this->swapchainImages.size();
        for (auto* model : models) {
            model->rebuild(swapchainImagesCount, this->_renderPass, this->_descriptorSetLayout, this->swapchainExtent);
        }
    }

    void Pipeline::releaseModels(std::vector<Model*>& models) const {
        const size_t swapchainImagesCount = this->swapchainImages.size();
        for (auto* model : models) {
            model->release(swapchainImagesCount);
        }
    }

    void Pipeline::createSwapchain() {
        vkUtils::SwapChainSupportDetails    swapChainSupport = vkUtils::QuerySwapChainSupport(this->_device->physicalDevice, this->_device->getAssociatedSurface());
        vkUtils::QueueFamilyIndices         indices = vkUtils::findQueueFamilies(this->_device->physicalDevice, this->_device->getAssociatedSurface());
        uint32_t    queueFamilyIndices[] = { indices.graphicsFamily.value(), indices.presentFamily.value() };

        const VkSurfaceFormatKHR surfaceFormat = vkUtils::ChooseSwapSurfaceFormat(swapChainSupport.formats);
        const VkPresentModeKHR presentMode = vkUtils::ChooseSwapPresentMode(swapChainSupport.presentMode);
        const VkExtent2D extent = vkUtils::ChooseSwapExtent(swapChainSupport.capabilities, this->_device->getSurfaceHeight(), this->_device->getSurfaceWidth());
        uint32_t imageCount = swapChainSupport.capabilities.minImageCount + 1;
        if (swapChainSupport.capabilities.maxImageCount > 0 && imageCount > swapChainSupport.capabilities.maxImageCount) {
            imageCount = swapChainSupport.capabilities.maxImageCount;
        }
        VkSwapchainCreateInfoKHR createInfo = {};
        createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
        createInfo.surface = this->_device->getAssociatedSurface();
        createInfo.minImageCount = imageCount;
        createInfo.imageFormat = surfaceFormat.format;
        createInfo.imageColorSpace = surfaceFormat.colorSpace;
        createInfo.imageExtent = extent;
        createInfo.imageArrayLayers = 1;
        createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
        if (indices.graphicsFamily != indices.presentFamily) {
            createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
            createInfo.queueFamilyIndexCount = 2;
            createInfo.pQueueFamilyIndices = queueFamilyIndices;
        }
        else {
            createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
            createInfo.queueFamilyIndexCount = 0;
            createInfo.pQueueFamilyIndices = nullptr;
        }
        createInfo.preTransform = swapChainSupport.capabilities.currentTransform;
        createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
        createInfo.presentMode = presentMode;
        createInfo.clipped = VK_TRUE;
        createInfo.oldSwapchain = nullptr;
        const VkResult result = vkCreateSwapchainKHR(this->_device->logicalDevice, &createInfo, nullptr, &this->swapchain);
        if (result != VK_SUCCESS) {
            throw std::runtime_error("Failed to create the swapchain.");
        }
        vkGetSwapchainImagesKHR(this->_device->logicalDevice, this->swapchain, &imageCount, nullptr);
        this->swapchainImages.resize(imageCount);
        vkGetSwapchainImagesKHR(this->_device->logicalDevice, this->swapchain, &imageCount, this->swapchainImages.data());
        this->_swapchainImageFormat = surfaceFormat.format;
        this->swapchainExtent = extent;
    }

    void Pipeline::createImageViews() {
        VkImageViewCreateInfo   createInfo = {};

        createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
        createInfo.format = this->_swapchainImageFormat;
        createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        createInfo.subresourceRange.baseMipLevel = 0;
        createInfo.subresourceRange.levelCount = 1;
        createInfo.subresourceRange.baseArrayLayer = 0;
        createInfo.subresourceRange.layerCount = 1;

        this->_swapchainImageViews.resize(this->swapchainImages.size());
        for (unsigned idx = 0; idx < this->_swapchainImageViews.size(); idx++) {
            createInfo.image = this->swapchainImages[idx];
            const VkResult result = vkCreateImageView(this->_device->logicalDevice,
                &createInfo,
                nullptr,
                &this->_swapchainImageViews[idx]);
            if (result != VK_SUCCESS) {
                throw std::runtime_error("Failed to create image views.");
            }
        }
    }

    void Pipeline::createRenderPass() {
        VkAttachmentDescription colorAttachment{};
        colorAttachment.format = this->_swapchainImageFormat;
        colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
        colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
        colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
        VkAttachmentReference colorAttachmentRef{};
        colorAttachmentRef.attachment = 0;
        colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
        VkSubpassDescription subpass{};
        subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
        subpass.colorAttachmentCount = 1;
        subpass.pColorAttachments = &colorAttachmentRef;
        VkSubpassDependency dependency{};
        dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
        dependency.dstSubpass = 0;
        dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        dependency.srcAccessMask = 0;
        dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
        VkRenderPassCreateInfo renderPassInfo{};
        renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
        renderPassInfo.attachmentCount = 1;
        renderPassInfo.pAttachments = &colorAttachment;
        renderPassInfo.subpassCount = 1;
        renderPassInfo.pSubpasses = &subpass;
        renderPassInfo.dependencyCount = 1;
        renderPassInfo.pDependencies = &dependency;
        const VkResult result = vkCreateRenderPass(this->_device->logicalDevice,
            &renderPassInfo,
            nullptr,
            &this->_renderPass);
        if (result != VK_SUCCESS) {
            throw std::runtime_error("Failed to create render pass!");
        }
    }

    void Pipeline::createFrameBuffers() {
        this->_swapchainFrameBuffers.resize(this->_swapchainImageViews.size());
        for (size_t i = 0; i < this->_swapchainImageViews.size(); i++) {
            VkImageView attachments[] = {
                this->_swapchainImageViews[i]
            };

            VkFramebufferCreateInfo frameBufferInfo{};
            frameBufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
            frameBufferInfo.renderPass = this->_renderPass;
            frameBufferInfo.attachmentCount = 1;
            frameBufferInfo.pAttachments = attachments;
            frameBufferInfo.width = this->swapchainExtent.width;
            frameBufferInfo.height = this->swapchainExtent.height;
            frameBufferInfo.layers = 1;

            if (vkCreateFramebuffer(this->_device->logicalDevice, &frameBufferInfo, nullptr, &this->_swapchainFrameBuffers[i]) != VK_SUCCESS) {
                throw std::runtime_error("failed to create frameBuffer!");
            }
        }
    }

    void Pipeline::createCommandPool() {
        vkUtils::QueueFamilyIndices queueFamilyIndices = vkUtils::findQueueFamilies(this->_device->physicalDevice, this->_device->getAssociatedSurface());

        VkCommandPoolCreateInfo poolInfo{};
        poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
        poolInfo.queueFamilyIndex = queueFamilyIndices.graphicsFamily.value();
        poolInfo.flags = 0;
        if (vkCreateCommandPool(this->_device->logicalDevice, &poolInfo, nullptr, &this->_commandPool) != VK_SUCCESS) {
            throw std::runtime_error("Failed to create command pool.");
        }
    }

    void Pipeline::createCommandBuffers(std::vector<Model*>& models) {
        this->commandBuffers.resize(this->_swapchainFrameBuffers.size());
        VkCommandBufferAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocInfo.commandPool = this->_commandPool;
        allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        allocInfo.commandBufferCount = static_cast<uint32_t>(this->commandBuffers.size());

        if (vkAllocateCommandBuffers(this->_device->logicalDevice, &allocInfo, this->commandBuffers.data()) != VK_SUCCESS) {
            throw std::runtime_error("Failed to allocate command buffers.");
        }
        for (size_t i = 0; i < this->commandBuffers.size(); i++) {
            VkCommandBufferBeginInfo beginInfo{};
            beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
            beginInfo.flags = 0;
            beginInfo.pInheritanceInfo = nullptr;

            if (vkBeginCommandBuffer(commandBuffers[i], &beginInfo) != VK_SUCCESS) {
                throw std::runtime_error("Failed to begin recording command buffer.");
            }
            VkClearValue clearColor = { 0.0f, 0.0f, 0.0f, 1.0f };
            VkRenderPassBeginInfo renderPassInfo{};
            renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
            renderPassInfo.renderPass = this->_renderPass;
            renderPassInfo.framebuffer = this->_swapchainFrameBuffers[i];
            renderPassInfo.renderArea.offset = { 0, 0 };
            renderPassInfo.renderArea.extent = this->swapchainExtent;
            renderPassInfo.clearValueCount = 1;
            renderPassInfo.pClearValues = &clearColor;
            vkCmdBeginRenderPass(this->commandBuffers[i], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
            for (auto* model: models) {
                model->commitRenderCommand(commandBuffers[i], this->_descriptorSets[i], i);
            }
            vkCmdEndRenderPass(this->commandBuffers[i]);
            if (vkEndCommandBuffer(this->commandBuffers[i]) != VK_SUCCESS) {
                throw std::runtime_error("Failed to record command buffer.");
            }
        }
    }
}

