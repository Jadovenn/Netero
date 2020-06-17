/**
 * Netero sources under BSD-3-Clause
 * see LICENSE.txt
 */

#include <stdexcept>
#include <netero/graphics/descriptor.hpp>

namespace netero::graphics {

    Descriptor::Descriptor(Device* device, uint32_t frameCount)
        :   _device(device),
            _frameCount(frameCount)
    {}

    Descriptor::~Descriptor() {}

    void Descriptor::setDescriptorType(VkDescriptorType descriptorType) {
        this->_descriptorType = descriptorType;
    }

    void Descriptor::setShaderStage(VkShaderStageFlags shaderStage) {
        this->_shaderStage = shaderStage;
    }

    void Descriptor::setBinding(uint32_t binding) {
        this->_binding = binding;
    }

    void Descriptor::build() {
        this->_createDescriptorPool();
        this->_createDescriptorSets();
        this->_createDescriptorLayout();
    }

    void Descriptor::rebuild() {
    }

    void Descriptor::release() {
    }

    // Noop but will be needed
    void Descriptor::update() {}

    // TODO: Move DescriptorPool in Pipeline
    // DescriptorPool could be hold by the pipeline a created
    // before any descriptor as we may know how much of them we need
    void Descriptor::_createDescriptorPool() {
        VkDescriptorPoolSize poolSize{};
        poolSize.type = this->_descriptorType;
        poolSize.descriptorCount = this->_frameCount;
        VkDescriptorPoolCreateInfo poolInfo{};
        poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
        poolInfo.poolSizeCount = 1;
        poolInfo.pPoolSizes = &poolSize;
        poolInfo.maxSets = this->_frameCount;
        if (vkCreateDescriptorPool(this->_device->logicalDevice, &poolInfo, nullptr, &this->_descriptorPool) != VK_SUCCESS) {
            throw std::runtime_error("failed to create descriptor pool!");
        }
    }

    void Descriptor::_createDescriptorLayout() {
        VkDescriptorSetLayoutBinding    uboBinding{};
        uboBinding.binding = this->_binding;
        uboBinding.descriptorCount = 1;
        uboBinding.descriptorType = this->_descriptorType;
        uboBinding.stageFlags = this->_shaderStage;
        uboBinding.pImmutableSamplers = nullptr;

        VkDescriptorSetLayoutCreateInfo layoutInfo{};
        layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
        layoutInfo.bindingCount = 1;
        layoutInfo.pBindings = &uboBinding;

        const VkResult result = vkCreateDescriptorSetLayout(this->_device->logicalDevice,
            &layoutInfo,
            nullptr,
            &this->_descriptorSetLayout);
        if (result != VK_SUCCESS) {
            throw std::runtime_error("Failed to create descriptor set layout.");
        }
    }

    void Descriptor::_createDescriptorSets() {
        std::vector<VkDescriptorSetLayout> layouts(this->_frameCount, this->_descriptorSetLayout);
        VkDescriptorSetAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
        allocInfo.descriptorPool = this->_descriptorPool;
        allocInfo.descriptorSetCount = this->_frameCount;
        allocInfo.pSetLayouts = layouts.data();
        this->_descriptorSets.resize(this->_frameCount);
        if (vkAllocateDescriptorSets(this->_device->logicalDevice, &allocInfo, this->_descriptorSets.data()) != VK_SUCCESS) {
            throw std::runtime_error("failed to allocate descriptor sets!");
        }
        for (size_t i = 0; i < this->_frameCount; i++) {
            VkDescriptorImageInfo   imageInfo{};
            imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
            imageInfo.imageView = this->_imageView;
            imageInfo.sampler = this->_textureSampler;
            VkWriteDescriptorSet descriptorWrite{};
            descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            descriptorWrite.dstSet = this->_descriptorSets[i];
            descriptorWrite.dstBinding = 1;
            descriptorWrite.dstArrayElement = 0;
            descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
            descriptorWrite.descriptorCount = 1;
            descriptorWrite.pBufferInfo = nullptr;
            descriptorWrite.pImageInfo = &imageInfo;
            descriptorWrite.pTexelBufferView = nullptr;
            vkUpdateDescriptorSets(this->_device->logicalDevice, 1, &descriptorWrite, 0, nullptr);
        }
    }

}

