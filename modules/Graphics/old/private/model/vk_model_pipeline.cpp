/**
 * Netero sources under BSD-3-Clause
 * see LICENSE.txt
 */

#include <stdexcept>

#include <netero/graphics/context.hpp>
#include <netero/graphics/instance.hpp>
#include <netero/graphics/model.hpp>

#include "utils/vkUtils.hpp"

namespace netero::graphics {

void Model::createInstanceBuffer(size_t framesCount)
{
    const size_t slot_count = this->_modelInstances.size() * framesCount;
    const size_t size = sizeof(Instance::Data) * slot_count;
    auto [buffer, bufferMemory] = vkUtils::AllocBuffer(this->_device,
                                                       size,
                                                       VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
                                                       VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                                                           VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
    void* data = nullptr;
    vkMapMemory(this->_device->logicalDevice, bufferMemory, 0, size, 0, &data);
    for (size_t idx = 0; idx < this->_modelInstances.size(); ++idx) {
        this->_modelInstances[idx]->offset = idx;
        Instance::Data* shared = this->_modelInstances[idx]->update();
        for (size_t frameIdx = 0; frameIdx < framesCount; ++frameIdx) {
            const size_t offset = frameIdx * this->_modelInstances.size() * sizeof(Instance::Data);
            std::memcpy(static_cast<char*>(data) + (offset + idx * sizeof(Instance::Data)),
                        shared,
                        sizeof(Instance::Data));
        }
    }
    vkUnmapMemory(this->_device->logicalDevice, bufferMemory);
    this->_instanceBuffer = buffer;
    this->_instanceBufferMemory = bufferMemory;
}

void Model::createDescriptors(uint32_t frameCount)
{
    this->_uboDescriptor.binding = 0;
    this->_uboDescriptor.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    this->_uboDescriptor.shaderStage = VK_SHADER_STAGE_VERTEX_BIT;
    this->_descriptorSets.add(this->_uboDescriptor);
    if (!this->_textures.empty()) {
        this->_textureDescriptor.binding = 1;
        this->_textureDescriptor.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        this->_textureDescriptor.shaderStage = VK_SHADER_STAGE_FRAGMENT_BIT;
        this->_descriptorSets.add(this->_textureDescriptor);
    }
    this->_descriptorSets.setSetsCount(frameCount);
    this->_descriptorSets.build();
}

void Model::writeToDescriptorSet(uint32_t frameCount, std::vector<VkBuffer>& uboBuffers)
{
    for (unsigned idx = 0; idx < frameCount; ++idx) {
        this->_descriptorSets.write(this->_uboDescriptor,
                                    static_cast<int>(idx),
                                    uboBuffers[idx],
                                    0,
                                    sizeof(UniformBufferObject));
    }
    if (!this->_textures.empty()) {
        this->_descriptorSets.write(this->_textureDescriptor,
                                    DescriptorSets::WHOLE_SIZE,
                                    this->_textures.getImageView(),
                                    this->_textures.getSampler());
    }
}

void Model::createGraphicsPipeline(VkRenderPass renderPass, VkExtent2D swapchainExtent)
{
    if (this->_modelInstances.empty()) {
        return;
    }
    std::vector<VkPipelineShaderStageCreateInfo> shaderStage;
    for (const auto& shader : this->_shaderModules) {
        VkPipelineShaderStageCreateInfo createInfo = {};
        createInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        switch (shader.stage) {
            case ShaderStage::VERTEX: createInfo.stage = VK_SHADER_STAGE_VERTEX_BIT; break;
            case ShaderStage::TESSELLATION:
                createInfo.stage = VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT;
                break;
            case ShaderStage::GEOMETRY: createInfo.stage = VK_SHADER_STAGE_GEOMETRY_BIT; break;
            case ShaderStage::FRAGMENT: createInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT; break;
            default:;
        }
        createInfo.module = shader.shaderModule;
        createInfo.pName = "main";
        shaderStage.push_back(createInfo);
    }
    std::vector<VkVertexInputBindingDescription> vertexBindings = {
        Vertex::getBindingDescription(),
        Instance::getBindingDescription()
    };
    auto vertexAttributeDescriptions = netero::graphics::Vertex::getAttributeDescription();
    auto instanceAttributeDescriptions = netero::graphics::Instance::getAttributeDescription();
    std::vector<VkVertexInputAttributeDescription> vertexAttributes = {
        vertexAttributeDescriptions[0],   vertexAttributeDescriptions[1],
        vertexAttributeDescriptions[2],   instanceAttributeDescriptions[0],
        instanceAttributeDescriptions[1], instanceAttributeDescriptions[2],
        instanceAttributeDescriptions[3],
    };
    VkPipelineVertexInputStateCreateInfo vertexInputInfo {};
    vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    vertexInputInfo.vertexBindingDescriptionCount = vertexBindings.size();
    vertexInputInfo.pVertexBindingDescriptions = vertexBindings.data();
    vertexInputInfo.vertexAttributeDescriptionCount =
        static_cast<uint32_t>(vertexAttributes.size());
    vertexInputInfo.pVertexAttributeDescriptions = vertexAttributes.data();
    VkPipelineInputAssemblyStateCreateInfo inputAssembly {};
    inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    inputAssembly.primitiveRestartEnable = VK_FALSE;
    VkViewport viewport {};
    viewport.x = 0.0f;
    viewport.y = 0.0f;
    viewport.width = static_cast<float>(swapchainExtent.width);
    viewport.height = static_cast<float>(swapchainExtent.height);
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;
    VkRect2D scissor {};
    scissor.offset = { 0, 0 };
    scissor.extent = swapchainExtent;
    VkPipelineViewportStateCreateInfo viewportState {};
    viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    viewportState.viewportCount = 1;
    viewportState.pViewports = &viewport;
    viewportState.scissorCount = 1;
    viewportState.pScissors = &scissor;
    VkPipelineRasterizationStateCreateInfo rasterizer {};
    rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    rasterizer.depthClampEnable = VK_FALSE;
    rasterizer.rasterizerDiscardEnable = VK_FALSE;
    rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
    rasterizer.lineWidth = 1.0f;
    rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
    rasterizer.frontFace = VK_FRONT_FACE_CLOCKWISE;
    rasterizer.depthBiasEnable = VK_FALSE;
    rasterizer.depthBiasConstantFactor = 0.0f;
    rasterizer.depthBiasClamp = 0.0f;
    rasterizer.depthBiasSlopeFactor = 0.0f;
    VkPipelineMultisampleStateCreateInfo multisampling {};
    multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    multisampling.sampleShadingEnable = VK_FALSE;
    multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
    multisampling.minSampleShading = 1.0f;
    multisampling.pSampleMask = nullptr;
    multisampling.alphaToCoverageEnable = VK_FALSE;
    multisampling.alphaToOneEnable = VK_FALSE;
    VkPipelineColorBlendAttachmentState colorBlendAttachment {};
    colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT |
        VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
    colorBlendAttachment.blendEnable = VK_FALSE;
    colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_ONE;
    colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO;
    colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;
    colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
    colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
    colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;
    VkPipelineColorBlendStateCreateInfo colorBlending {};
    colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    colorBlending.logicOpEnable = VK_FALSE;
    colorBlending.logicOp = VK_LOGIC_OP_COPY;
    colorBlending.attachmentCount = 1;
    colorBlending.pAttachments = &colorBlendAttachment;
    colorBlending.blendConstants[0] = 0.0f;
    colorBlending.blendConstants[1] = 0.0f;
    colorBlending.blendConstants[2] = 0.0f;
    colorBlending.blendConstants[3] = 0.0f;
    VkDynamicState dynamicStates[] = { VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_LINE_WIDTH };
    VkPipelineDynamicStateCreateInfo dynamicState {};
    dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
    dynamicState.dynamicStateCount = 2;
    dynamicState.pDynamicStates = dynamicStates;
    VkPipelineLayoutCreateInfo pipelineLayoutInfo {};
    pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipelineLayoutInfo.setLayoutCount = 1;
    pipelineLayoutInfo.pSetLayouts = this->_descriptorSets.getLayout();
    pipelineLayoutInfo.pushConstantRangeCount = 0;
    pipelineLayoutInfo.pPushConstantRanges = nullptr;
    if (vkCreatePipelineLayout(this->_device->logicalDevice,
                               &pipelineLayoutInfo,
                               nullptr,
                               &this->_pipelineLayout) != VK_SUCCESS) {
        throw std::runtime_error("failed to create pipeline layout!");
    }
    VkPipelineDepthStencilStateCreateInfo depthStencil {};
    depthStencil.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
    depthStencil.depthTestEnable = VK_TRUE;
    depthStencil.depthWriteEnable = VK_TRUE;
    depthStencil.depthCompareOp = VK_COMPARE_OP_LESS;
    depthStencil.depthBoundsTestEnable = VK_FALSE;
    depthStencil.minDepthBounds = 0.0f; // Optional
    depthStencil.maxDepthBounds = 1.0f; // Optional
    depthStencil.stencilTestEnable = VK_FALSE;
    //depthStencil.front{}; // Optional
    //depthStencil.back{}; // Optional
    VkGraphicsPipelineCreateInfo pipelineInfo {};
    pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    pipelineInfo.stageCount = shaderStage.size();
    pipelineInfo.pStages = shaderStage.data();
    pipelineInfo.pVertexInputState = &vertexInputInfo;
    pipelineInfo.pInputAssemblyState = &inputAssembly;
    pipelineInfo.pViewportState = &viewportState;
    pipelineInfo.pRasterizationState = &rasterizer;
    pipelineInfo.pMultisampleState = &multisampling;
    pipelineInfo.pDepthStencilState = &depthStencil;
    pipelineInfo.pColorBlendState = &colorBlending;
    pipelineInfo.pDynamicState = nullptr;
    pipelineInfo.layout = this->_pipelineLayout;
    pipelineInfo.renderPass = renderPass;
    pipelineInfo.subpass = 0;
    pipelineInfo.basePipelineHandle = nullptr;
    pipelineInfo.basePipelineIndex = -1;
    if (vkCreateGraphicsPipelines(this->_device->logicalDevice,
                                  nullptr,
                                  1,
                                  &pipelineInfo,
                                  nullptr,
                                  &this->_graphicsPipeline) != VK_SUCCESS) {
        throw std::runtime_error("failed to create graphics pipeline!");
    }
}

void Model::commitRenderCommand(VkCommandBuffer cmdBuffer, size_t frameIdx)
{
    if (this->_modelInstances.empty()) {
        return;
    }
    vkCmdBindPipeline(cmdBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, this->_graphicsPipeline);
    VkBuffer     vertexBuffer[] = { this->_vertexBuffer.vertexBuffer };
    VkDeviceSize offsets[] = { 0 };
    VkBuffer     instanceBuffer[] = { this->_instanceBuffer };
    VkDeviceSize instanceOffsets[] = { sizeof(Instance::Data) * this->_modelInstances.size() *
                                       frameIdx };
    vkCmdBindVertexBuf;
    vkCmdBindVertexBuffers(cmdBuffer, 1, 1, instanceBuffer, instanceOffsets);
    vkCmdBindIndexBuffer(cmdBuffer, this->_vertexBuffer.indexBuffer, 0, VK_INDEX_TYPE_UINT32);
    vkCmdBindDescriptorSets(cmdBuffer,
                            VK_PIPELINE_BIND_POINT_GRAPHICS,
                            this->_pipelineLayout,
                            0,
                            1,
                            this->_descriptorSets.at(frameIdx),
                            0,
                            nullptr);
    vkCmdDrawIndexed(cmdBuffer,
                     static_cast<uint32_t>(this->_vertexBuffer.indices.size()),
                     this->_modelInstances.size(),
                     0,
                     0,
                     0);
}

void Model::update(uint32_t frameIndex)
{
    void* data = nullptr;
    vkMapMemory(this->_device->logicalDevice,
                this->_instanceBufferMemory,
                frameIndex * this->_modelInstances.size() * sizeof(Instance::Data),
                this->_modelInstances.size() * sizeof(Instance::Data),
                0,
                &data);
    for (size_t idx = 0; idx < this->_modelInstances.size(); ++idx) {
        Instance::Data* shared = this->_modelInstances[idx]->update();
        std::memcpy(static_cast<char*>(data) + (idx * sizeof(Instance::Data)),
                    shared,
                    sizeof(Instance::Data));
    }
    vkUnmapMemory(this->_device->logicalDevice, this->_instanceBufferMemory);
}

} // namespace netero::graphics
