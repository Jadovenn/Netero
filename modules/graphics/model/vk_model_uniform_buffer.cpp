/**
 * Netero sources under BSD-3-Clause
 * see LICENSE.txt
 */

#include <stdexcept>
#include <chrono>
#include <netero/graphics/model.hpp>
#include "utils/vkUtils.hpp"

namespace netero::graphics {

    void Model::createUniformBuffers(size_t swapchainImagesCount) {
        const VkDeviceSize    size = sizeof(UniformBufferObject) * this->_modelInstances.size();
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

    void Model::createDescriptorPool(size_t swapchainImagesCount) {
        VkDescriptorPoolSize poolSize{};
        poolSize.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        poolSize.descriptorCount = static_cast<uint32_t>(swapchainImagesCount);
        VkDescriptorPoolCreateInfo poolInfo{};
        poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
        poolInfo.poolSizeCount = 1;
        poolInfo.pPoolSizes = &poolSize;
        poolInfo.maxSets = swapchainImagesCount;
        if (vkCreateDescriptorPool(this->_device->logicalDevice, &poolInfo, nullptr, &this->_descriptorPool) != VK_SUCCESS) {
            throw std::runtime_error("failed to create descriptor pool!");
        }
    }

    void Model::createDescriptorSets(size_t swapchainImagesCount) {
        std::vector<VkDescriptorSetLayout> layouts(swapchainImagesCount, this->_descriptorSetLayout);
        VkDescriptorSetAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
        allocInfo.descriptorPool = this->_descriptorPool;
        allocInfo.descriptorSetCount = static_cast<uint32_t>(swapchainImagesCount);
        allocInfo.pSetLayouts = layouts.data();
        this->_descriptorSets.resize(swapchainImagesCount);
        if (vkAllocateDescriptorSets(this->_device->logicalDevice, &allocInfo, this->_descriptorSets.data()) != VK_SUCCESS) {
            throw std::runtime_error("failed to allocate descriptor sets!");
        }
        for (size_t i = 0; i < swapchainImagesCount; i++) {
            VkDescriptorBufferInfo bufferInfo{};
            bufferInfo.buffer = uniformBuffers[i];
            bufferInfo.offset = 0;
            bufferInfo.range = sizeof(UniformBufferObject) * this->_modelInstances.size();
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

    void Model::createDescriptorSetLayout() {
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

    void Model::updateMVP(uint32_t frameIndex, VkExtent2D swapchainExtent) {
        UniformBufferObject* data = nullptr;
        vkMapMemory(this->_device->logicalDevice,
            this->uniformBuffersMemory[frameIndex],
            0,
            sizeof(UniformBufferObject) * this->_modelInstances.size(),
            0,
            reinterpret_cast<void**>(&data));
        for (size_t idx = 0; idx < this->_modelInstances.size(); ++idx) {
            UniformBufferObject ubo{};
            ubo.view = glm::lookAt(glm::vec3(0.f, 0.f, 2.f),
                glm::vec3(0.f, 0.f, 0.f),
                glm::vec3(0.f, 1.f, 0.f));
            ubo.proj = glm::perspectiveRH(glm::radians(45.f),
                swapchainExtent.width / static_cast<float>(swapchainExtent.height),
                0.1f,
                100.f);
            memcpy(&data[idx], &ubo, sizeof(ubo));
        }
        vkUnmapMemory(this->_device->logicalDevice,
            this->uniformBuffersMemory[frameIndex]);
    }
}

