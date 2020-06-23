/**
 * Netero sources under BSD-3-Clause
 * see LICENSE.txt
 */

#include <cassert>
#include <stdexcept>
#include <netero/graphics/descriptor_set.hpp>

namespace netero::graphics {

    const int DescriptorSets::WHOLE_SIZE = -1;

    DescriptorSets::DescriptorSets(Device* device, uint32_t descriptorSetCount)
        :   _device(device),
            _descriptorCount(descriptorSetCount)
    {
        assert(_device != nullptr);
    }

    DescriptorSets::~DescriptorSets() {
        this->release();
        vkDestroyDescriptorSetLayout(this->_device->logicalDevice, this->_descriptorSetLayout, nullptr);
    }

    void DescriptorSets::setSetsCount(uint32_t descriptorSetCount) {

        this->_descriptorCount = descriptorSetCount;
    }

    void DescriptorSets::add(Descriptor& descriptor) {
        this->_descriptors.push_back(descriptor);
    }

    void DescriptorSets::build() {
        this->_descriptors.shrink_to_fit();
        this->_createDescriptorSetLayout();
        this->_createDescriptorSetPool();
        this->_createDescriptorSets();
    }

    void DescriptorSets::rebuild() {
        this->release();
        this->_createDescriptorSetPool();
        this->_createDescriptorSets();
    }

    VkDescriptorSetLayout* DescriptorSets::getLayout() {
        return &this->_descriptorSetLayout;
    }

    VkDescriptorSet* DescriptorSets::at(uint32_t index) {
        if (index >= this->_descriptorSets.size()) {
            return nullptr;
        }
        return &this->_descriptorSets[index];
    }

    void DescriptorSets::release() {
        vkDestroyDescriptorPool(this->_device->logicalDevice, this->_descriptorPool, nullptr);
    }

    void DescriptorSets::_createDescriptorSetPool() {
        std::vector<VkDescriptorPoolSize>   poolSizes;
        for (auto& desc: this->_descriptors) {
            VkDescriptorPoolSize poolSize{};
            poolSize.type = desc.descriptorType;
            poolSize.descriptorCount = this->_descriptorCount;
            poolSizes.push_back(poolSize);
        }
        VkDescriptorPoolCreateInfo poolInfo{};
        poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
        poolInfo.poolSizeCount = poolSizes.size();
        poolInfo.pPoolSizes = poolSizes.data();
        poolInfo.maxSets = this->_descriptorCount;
        if (vkCreateDescriptorPool(this->_device->logicalDevice, &poolInfo, nullptr, &this->_descriptorPool) != VK_SUCCESS) {
            throw std::runtime_error("failed to create descriptor pool!");
        }
    }

    void DescriptorSets::_createDescriptorSetLayout() {
        std::vector<VkDescriptorSetLayoutBinding>   bindings;

        for (auto& desc: this->_descriptors) {
            VkDescriptorSetLayoutBinding    binding{};
            binding.binding = desc.binding;
            binding.descriptorCount = this->_descriptorCountPerSet;
            binding.descriptorType = desc.descriptorType;
            binding.stageFlags = desc.shaderStage;
            binding.pImmutableSamplers = nullptr;
            bindings.push_back(binding);
        }

        VkDescriptorSetLayoutCreateInfo layoutInfo{};
        layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
        layoutInfo.bindingCount = bindings.size();
        layoutInfo.pBindings = bindings.data();

        const VkResult result = vkCreateDescriptorSetLayout(this->_device->logicalDevice,
            &layoutInfo,
            nullptr,
            &this->_descriptorSetLayout);
        if (result != VK_SUCCESS) {
            throw std::runtime_error("Failed to create descriptor set layout.");
        }
    }

    void DescriptorSets::_createDescriptorSets() {
        std::vector<VkDescriptorSetLayout> layouts(this->_descriptorCount, this->_descriptorSetLayout);
        VkDescriptorSetAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
        allocInfo.descriptorPool = this->_descriptorPool;
        allocInfo.descriptorSetCount = this->_descriptorCount;
        allocInfo.pSetLayouts = layouts.data();
        this->_descriptorSets.resize(this->_descriptorCount);
        if (vkAllocateDescriptorSets(this->_device->logicalDevice, &allocInfo, this->_descriptorSets.data()) != VK_SUCCESS) {
            throw std::runtime_error("failed to allocate descriptor sets!");
        }
    }

    DescriptorSets::Error    DescriptorSets::write(Descriptor& descriptor, int setIdx, VkImageView imageView, VkSampler sampler) {
        if (this->_descriptorSets.empty()) {
            return Error::EMPTY_DESCRIPTOR_SET;
        }
        if (descriptor.descriptorType != VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER) {
            return Error::BAD_DESCRIPTOR_TYPE;
        }
        if (setIdx != WHOLE_SIZE && setIdx < 0 && setIdx >= this->_descriptorSets.size()) {
            return Error::INDEX_OUT_OF_BOUND;
        }
        VkDescriptorImageInfo   imageInfo;
        imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        imageInfo.imageView = imageView;
        imageInfo.sampler = sampler;
        this->_write(descriptor, setIdx, nullptr, &imageInfo);
        return Error::SUCCESS;
    }

    DescriptorSets::Error    DescriptorSets::write(Descriptor& descriptor, int setIdx, VkBuffer buffer, VkDeviceSize offset, VkDeviceSize range) {
        if (this->_descriptorSets.empty()) {
            return Error::EMPTY_DESCRIPTOR_SET;
        }
        if (descriptor.descriptorType != VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER) {
            return Error::BAD_DESCRIPTOR_TYPE;
        }
        if (setIdx != WHOLE_SIZE && setIdx < 0 && setIdx >= this->_descriptorSets.size()) {
            return Error::INDEX_OUT_OF_BOUND;
        }
        VkDescriptorBufferInfo bufferInfo;
        bufferInfo.buffer = buffer;
        bufferInfo.offset = offset;
        bufferInfo.range = range;
        this->_write(descriptor, setIdx, &bufferInfo, nullptr);
        return Error::SUCCESS;
    }

    DescriptorSets::Error    DescriptorSets::_write(Descriptor& descriptor, int setIdx, VkDescriptorBufferInfo* bufferInfo, VkDescriptorImageInfo* imageInfo) {
        if (setIdx == WHOLE_SIZE) {
            for (size_t i = 0; i < this->_descriptorCount; i++) {
                VkWriteDescriptorSet descriptorWrite{};
                descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
                descriptorWrite.dstSet = this->_descriptorSets[i];
                descriptorWrite.dstArrayElement = 0;
                descriptorWrite.dstBinding = descriptor.binding;
                descriptorWrite.descriptorType = descriptor.descriptorType;
                descriptorWrite.descriptorCount = this->_descriptorCountPerSet;
                descriptorWrite.pBufferInfo = bufferInfo ? bufferInfo : nullptr;
                descriptorWrite.pImageInfo = imageInfo ? imageInfo : nullptr;
                descriptorWrite.pTexelBufferView = nullptr;
                vkUpdateDescriptorSets(this->_device->logicalDevice, 1, &descriptorWrite, 0, nullptr);
            }
        }
        else {
            VkWriteDescriptorSet descriptorWrite{};
            descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            descriptorWrite.dstSet = this->_descriptorSets[setIdx];
            descriptorWrite.dstBinding = descriptor.binding;
            descriptorWrite.dstArrayElement = 0;
            descriptorWrite.descriptorType = descriptor.descriptorType;
            descriptorWrite.descriptorCount = this->_descriptorCountPerSet;
            descriptorWrite.pBufferInfo = bufferInfo ? bufferInfo : nullptr;
            descriptorWrite.pImageInfo = imageInfo ? imageInfo : nullptr;
            descriptorWrite.pTexelBufferView = nullptr;
            vkUpdateDescriptorSets(this->_device->logicalDevice, 1, &descriptorWrite, 0, nullptr);
        }
        return Error::SUCCESS;
    }
}

