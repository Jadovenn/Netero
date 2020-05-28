/**
 * Netero sources under BSD-3-Clause
 * see LICENSE.txt
 */

#include <cassert>
#include <stdexcept>
#include <netero/graphics/pipeline.hpp>
#include <netero/graphics/vertex.hpp>

#include "utils/vkUtils.hpp"

namespace netero::graphics {

    Pipeline::Pipeline(VkInstance instance, Device* device)
        :   _instance(instance),
            _device(device),
            _renderPass(nullptr),
            _pipelineLayout(nullptr),
            _graphicsPipeline(nullptr),
            _commandPool(nullptr),
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

    void Pipeline::build(std::vector<Shader>& shaders, VertexBuffer& vtBuffer) {
        this->createSwapchain();
        this->createUniformBuffers();
        this->createDescriptorPool();
        this->createDescriptorSetLayout();
        this->createDescriptorSets();
        this->createImageViews();
        this->createRenderPass();
        this->createGraphicsPipeline(shaders);
        this->createFrameBuffers();
        this->createCommandPool();
        this->createCommandBuffers(vtBuffer);
    }

    void Pipeline::release() {
        for (auto* frameBuffer : this->_swapchainFrameBuffers) {
            vkDestroyFramebuffer(this->_device->logicalDevice, frameBuffer, nullptr);
        }
        vkFreeCommandBuffers(this->_device->logicalDevice, this->_commandPool, static_cast<uint32_t>(this->commandBuffers.size()), this->commandBuffers.data());
        vkDestroyPipeline(this->_device->logicalDevice, this->_graphicsPipeline, nullptr);
        vkDestroyPipelineLayout(this->_device->logicalDevice, this->_pipelineLayout, nullptr);
        vkDestroyRenderPass(this->_device->logicalDevice, this->_renderPass, nullptr);
        for (auto* imageView : this->_swapchainImageViews) {
            vkDestroyImageView(this->_device->logicalDevice, imageView, nullptr);
        }
        vkDestroySwapchainKHR(this->_device->logicalDevice, this->swapchain, nullptr);
        for (size_t idx = 0; idx < this->swapchainImages.size(); ++idx) {
            vkDestroyBuffer(this->_device->logicalDevice, uniformBuffers[idx], nullptr);
            vkFreeMemory(this->_device->logicalDevice, uniformBuffersMemory[idx], nullptr);
        }
        vkDestroyDescriptorPool(this->_device->logicalDevice, this->_descriptorPool, nullptr);
    }

    void Pipeline::rebuild(std::vector<Shader>& shaders, VertexBuffer& vtBuffer) {
        this->release();
        this->createSwapchain();
        this->createUniformBuffers();
        this->createDescriptorPool();
        this->createDescriptorSets();
        this->createImageViews();
        this->createRenderPass();
        this->createGraphicsPipeline(shaders);
        this->createFrameBuffers();
        this->createCommandBuffers(vtBuffer);
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

    // TODO: Do not allocate multiple buffers, allocate one buffer and use offsets
    void Pipeline::createUniformBuffers() {
        const VkDeviceSize    size = sizeof(UniformBufferObject);
        const size_t swapchainImagesCount = this->swapchainImages.size();
        this->uniformBuffers.resize(swapchainImagesCount);
        this->uniformBuffersMemory.resize(swapchainImagesCount);
        for (size_t idx = 0; idx < swapchainImagesCount; ++idx) {
            auto [buffer, bufferMemory] = vkUtils::AllocBuffer(this->_device,
                size,
                VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
            this->uniformBuffers[idx] = buffer;
            this->uniformBuffersMemory[idx] = bufferMemory;
        }
    }

    void Pipeline::createDescriptorPool() {
        VkDescriptorPoolSize poolSize{};
        poolSize.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        poolSize.descriptorCount = static_cast<uint32_t>(this->swapchainImages.size());
        VkDescriptorPoolCreateInfo poolInfo{};
        poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
        poolInfo.poolSizeCount = 1;
        poolInfo.pPoolSizes = &poolSize;
        poolInfo.maxSets = static_cast<uint32_t>(this->swapchainImages.size());
        if (vkCreateDescriptorPool(this->_device->logicalDevice , &poolInfo, nullptr, &this->_descriptorPool) != VK_SUCCESS) {
            throw std::runtime_error("failed to create descriptor pool!");
        }
    }

    void Pipeline::createDescriptorSets() {
        std::vector<VkDescriptorSetLayout> layouts(this->swapchainImages.size(), this->_descriptorSetLayout);
        VkDescriptorSetAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
        allocInfo.descriptorPool = this->_descriptorPool;
        allocInfo.descriptorSetCount = static_cast<uint32_t>(this->swapchainImages.size());
        allocInfo.pSetLayouts = layouts.data();
        this->_descriptorSets.resize(this->swapchainImages.size());
        if (vkAllocateDescriptorSets(this->_device->logicalDevice, &allocInfo, this->_descriptorSets.data()) != VK_SUCCESS) {
            throw std::runtime_error("failed to allocate descriptor sets!");
        }
        for (size_t i = 0; i < this->swapchainImages.size(); i++) {
            VkDescriptorBufferInfo bufferInfo{};
            bufferInfo.buffer = uniformBuffers[i];
            bufferInfo.offset = 0;
            bufferInfo.range = sizeof(UniformBufferObject);
            VkWriteDescriptorSet descriptorWrite{};
            descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            descriptorWrite.dstSet = this->_descriptorSets[i];
            descriptorWrite.dstBinding = 0;
            descriptorWrite.dstArrayElement = 0;
            descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
            descriptorWrite.descriptorCount = 1;
            descriptorWrite.pBufferInfo = &bufferInfo;
            descriptorWrite.pImageInfo = nullptr; // Optional
            descriptorWrite.pTexelBufferView = nullptr; // Optional
            vkUpdateDescriptorSets(this->_device->logicalDevice, 1, &descriptorWrite, 0, nullptr);
        }
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

    void Pipeline::createDescriptorSetLayout() {
        VkDescriptorSetLayoutBinding    uboBinding{};
        uboBinding.binding = 0;
        uboBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        uboBinding.descriptorCount = 1;
        uboBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
        uboBinding.pImmutableSamplers = nullptr;

        VkDescriptorSetLayoutCreateInfo layoutInfo{};
        layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
        layoutInfo.bindingCount = 1;
        layoutInfo.pBindings = &uboBinding;

        VkResult result = vkCreateDescriptorSetLayout(this->_device->logicalDevice, &layoutInfo, nullptr, &this->_descriptorSetLayout);
        if (result != VK_SUCCESS) {
            throw std::runtime_error("Failed to create descriptor set layout.");
        }
    }

    void Pipeline::createGraphicsPipeline(std::vector<Shader> &shaders) {
        std::vector<VkPipelineShaderStageCreateInfo> shaderStage;
        for (const auto& shader : shaders) {
            VkPipelineShaderStageCreateInfo createInfo = {};
            createInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
            switch (shader.stage) {
            case ShaderStage::VERTEX:
                createInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
                break;
            case ShaderStage::TESSELLATION:
                createInfo.stage = VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT;
                break;
            case ShaderStage::GEOMETRY:
                createInfo.stage = VK_SHADER_STAGE_GEOMETRY_BIT;
                break;
            case ShaderStage::FRAGMENT:
                createInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
                break;
            default:;
            }
            createInfo.module = shader.shaderModule;
            createInfo.pName = "main";
            shaderStage.push_back(createInfo);
        }

        auto bindingDescription = netero::graphics::Vertex::getBindingDescription();
        auto attributeDescriptions = netero::graphics::Vertex::getAttributeDescription();
        VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
        vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
        vertexInputInfo.vertexBindingDescriptionCount = 1;
        vertexInputInfo.pVertexBindingDescriptions = &bindingDescription;
        vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescriptions.size());
        vertexInputInfo.pVertexAttributeDescriptions = attributeDescriptions.data();
        VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
        inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
        inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
        inputAssembly.primitiveRestartEnable = VK_FALSE;
        VkViewport viewport{};
        viewport.x = 0.0f;
        viewport.y = 0.0f;
        viewport.width = static_cast<float>(this->swapchainExtent.width);
        viewport.height = static_cast<float>(this->swapchainExtent.height);
        viewport.minDepth = 0.0f;
        viewport.maxDepth = 1.0f;
        VkRect2D scissor{};
        scissor.offset = { 0, 0 };
        scissor.extent = this->swapchainExtent;
        VkPipelineViewportStateCreateInfo viewportState{};
        viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
        viewportState.viewportCount = 1;
        viewportState.pViewports = &viewport;
        viewportState.scissorCount = 1;
        viewportState.pScissors = &scissor;
        VkPipelineRasterizationStateCreateInfo rasterizer{};
        rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
        rasterizer.depthClampEnable = VK_FALSE;
        rasterizer.rasterizerDiscardEnable = VK_FALSE;
        rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
        rasterizer.lineWidth = 1.0f;
        rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
        rasterizer.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
        rasterizer.depthBiasEnable = VK_FALSE;
        rasterizer.depthBiasConstantFactor = 0.0f;
        rasterizer.depthBiasClamp = 0.0f;
        rasterizer.depthBiasSlopeFactor = 0.0f;
        VkPipelineMultisampleStateCreateInfo multisampling{};
        multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
        multisampling.sampleShadingEnable = VK_FALSE;
        multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
        multisampling.minSampleShading = 1.0f;
        multisampling.pSampleMask = nullptr;
        multisampling.alphaToCoverageEnable = VK_FALSE;
        multisampling.alphaToOneEnable = VK_FALSE;
        VkPipelineColorBlendAttachmentState colorBlendAttachment{};
        colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
        colorBlendAttachment.blendEnable = VK_FALSE;
        colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_ONE;
        colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO;
        colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;
        colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
        colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
        colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;
        VkPipelineColorBlendStateCreateInfo colorBlending{};
        colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
        colorBlending.logicOpEnable = VK_FALSE;
        colorBlending.logicOp = VK_LOGIC_OP_COPY;
        colorBlending.attachmentCount = 1;
        colorBlending.pAttachments = &colorBlendAttachment;
        colorBlending.blendConstants[0] = 0.0f;
        colorBlending.blendConstants[1] = 0.0f;
        colorBlending.blendConstants[2] = 0.0f;
        colorBlending.blendConstants[3] = 0.0f;
        VkDynamicState dynamicStates[] = {
            VK_DYNAMIC_STATE_VIEWPORT,
            VK_DYNAMIC_STATE_LINE_WIDTH
        };
        VkPipelineDynamicStateCreateInfo dynamicState{};
        dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
        dynamicState.dynamicStateCount = 2;
        dynamicState.pDynamicStates = dynamicStates;
        VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
        pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        pipelineLayoutInfo.setLayoutCount = 1;
        pipelineLayoutInfo.pSetLayouts = &this->_descriptorSetLayout;
        pipelineLayoutInfo.pushConstantRangeCount = 0;
        pipelineLayoutInfo.pPushConstantRanges = nullptr;
        if (vkCreatePipelineLayout(this->_device->logicalDevice, &pipelineLayoutInfo, nullptr, &this->_pipelineLayout) != VK_SUCCESS) {
            throw std::runtime_error("failed to create pipeline layout!");
        }
        VkGraphicsPipelineCreateInfo pipelineInfo{};
        pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
        pipelineInfo.stageCount = shaderStage.size();
        pipelineInfo.pStages = shaderStage.data();
        pipelineInfo.pVertexInputState = &vertexInputInfo;
        pipelineInfo.pInputAssemblyState = &inputAssembly;
        pipelineInfo.pViewportState = &viewportState;
        pipelineInfo.pRasterizationState = &rasterizer;
        pipelineInfo.pMultisampleState = &multisampling;
        pipelineInfo.pDepthStencilState = nullptr;
        pipelineInfo.pColorBlendState = &colorBlending;
        pipelineInfo.pDynamicState = nullptr;
        pipelineInfo.layout = this->_pipelineLayout;
        pipelineInfo.renderPass = this->_renderPass;
        pipelineInfo.subpass = 0;
        pipelineInfo.basePipelineHandle = nullptr;
        pipelineInfo.basePipelineIndex = -1;
        if (vkCreateGraphicsPipelines(this->_device->logicalDevice, nullptr, 1, &pipelineInfo, nullptr, &this->_graphicsPipeline) != VK_SUCCESS) {
            throw std::runtime_error("failed to create graphics pipeline!");
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

    void Pipeline::createCommandBuffers(VertexBuffer& vtBuffer) {
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
            vkCmdBindPipeline(this->commandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, this->_graphicsPipeline);
            VkBuffer vertexBuffer[] = { vtBuffer.vertexBuffer };
            VkDeviceSize    offsets[] = { 0 };
            vkCmdBindVertexBuffers(this->commandBuffers[i], 0, 1, vertexBuffer, offsets);
            vkCmdBindIndexBuffer(commandBuffers[i], vtBuffer.indexBuffer, 0, VK_INDEX_TYPE_UINT16);
            vkCmdBindDescriptorSets(commandBuffers[i],
                VK_PIPELINE_BIND_POINT_GRAPHICS,
                this->_pipelineLayout,
                0,
                1,
                &this->_descriptorSets[i],
                0,
                nullptr);
            vkCmdDrawIndexed(this->commandBuffers[i], static_cast<uint32_t>(vtBuffer.indices.size()), 1, 0, 0, 0);
            vkCmdEndRenderPass(this->commandBuffers[i]);
            if (vkEndCommandBuffer(this->commandBuffers[i]) != VK_SUCCESS) {
                throw std::runtime_error("Failed to record command buffer.");
            }
        }
    }
}

