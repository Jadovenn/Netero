/**
 * Netero sources under BSD-3-Clause
 * see LICENSE.txt
 */

#include <stdexcept>
#include <netero/extra/stb_image.h>
#include <netero/graphics/texture.hpp>

#include "utils/vkUtils.hpp"

namespace netero::graphics {

    Texture::Texture(Device* device)
        :   _device(device)
    {}

    Texture::~Texture() {
        for (const auto& texture: textures) {
            if (texture.stagingBuffer.first) {
                vkDestroyBuffer(this->_device->logicalDevice, texture.stagingBuffer.first, nullptr);
                vkFreeMemory(this->_device->logicalDevice, texture.stagingBuffer.second, nullptr);
            }
            if (texture.image.first) {
                vkDestroyImageView(this->_device->logicalDevice, texture.imageView, nullptr);
                vkDestroyImage(this->_device->logicalDevice, texture.image.first, nullptr);
                vkFreeMemory(this->_device->logicalDevice, texture.image.second, nullptr);
            }
        }
    }

    void Texture::transferTextureToGPU() {
        VkCommandBuffer transferCmd = vkUtils::BeginCommandBufferRecording(this->_device->logicalDevice,
            this->_device->transferCommandPool,
            VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);

        for (auto& texture : this->textures) {
            auto [image, imageMemory] = vkUtils::AllocImage(this->_device,
                texture.width,
                texture.height,
                VK_FORMAT_R8G8B8A8_SRGB,
                VK_IMAGE_TILING_OPTIMAL,
                VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
                VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
            texture.image.first = image;
            texture.image.second = imageMemory;
            vkUtils::TransitionImageLayout(transferCmd,
                image,
                VK_FORMAT_R8G8B8A8_SRGB,
                VK_IMAGE_LAYOUT_UNDEFINED,
                VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
            vkUtils::TransferImage(transferCmd,
                texture.stagingBuffer.first,
                texture.image.first,
                texture.width,
                texture.height);
            vkUtils::TransitionImageLayout(transferCmd,
                texture.image.first,
                VK_FORMAT_R8G8B8A8_SRGB,
                VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

        }
        vkUtils::FlushCommandBuffer(this->_device->logicalDevice,
            this->_device->transferQueue,
            this->_device->transferCommandPool,
            transferCmd);
        for (auto& texture : this->textures) {
            vkDestroyBuffer(this->_device->logicalDevice, texture.stagingBuffer.first, nullptr);
            vkFreeMemory(this->_device->logicalDevice, texture.stagingBuffer.second, nullptr);
            texture.stagingBuffer.first = nullptr;
            texture.stagingBuffer.second = nullptr;
        }
    }

    void Texture::createTexturesView() {
        for (auto& texture : this->textures) {
            texture.imageView = vkUtils::CreateImageView(this->_device->logicalDevice,
                texture.image.first,
                VK_FORMAT_R8G8B8A8_SRGB);
        }
    }

    void Texture::createTexturesSampler() {
        for (auto& texture : this->textures) {
            VkSamplerCreateInfo samplerInfo{};
            samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
            samplerInfo.magFilter = VK_FILTER_LINEAR;
            samplerInfo.minFilter = VK_FILTER_LINEAR;
            samplerInfo.addressModeU = static_cast<VkSamplerAddressMode>(texture.samplingMode);
            samplerInfo.addressModeV = static_cast<VkSamplerAddressMode>(texture.samplingMode);
            samplerInfo.addressModeW = static_cast<VkSamplerAddressMode>(texture.samplingMode);
            samplerInfo.anisotropyEnable = VK_TRUE;
            samplerInfo.maxAnisotropy = 16.0f;
            samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
            samplerInfo.unnormalizedCoordinates = VK_FALSE;
            samplerInfo.compareEnable = VK_FALSE;
            samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;
            samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
            samplerInfo.mipLodBias = 0.0f;
            samplerInfo.minLod = 0.0f;
            samplerInfo.maxLod = 0.0f;
            if (vkCreateSampler(this->_device->logicalDevice, &samplerInfo, nullptr, &texture.textureSampler) != VK_SUCCESS) {
                throw std::runtime_error("failed to create texture sampler!");
            }
        } 
    }

    void Texture::loadTexture(const std::string& path, TextureSamplingMode samplingMode) {
        int width, height, channels = 0;
        stbi_uc* pixels = stbi_load(path.c_str(),
            &width,
            &height,
            &channels,
            STBI_rgb_alpha);
        const VkDeviceSize imageSize = width * height * 4;
        if (!pixels) {
            throw std::runtime_error("Failed to load texture from fs.");
        }
        auto [stagingBuffer, stagingBufferMemory] = vkUtils::AllocBuffer(this->_device,
            imageSize,
            VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
        void* data;
        vkMapMemory(this->_device->logicalDevice,
            stagingBufferMemory,
            0,
            imageSize,
            0,
            &data);
        memcpy(data, pixels, static_cast<size_t>(imageSize));
        vkUnmapMemory(this->_device->logicalDevice, stagingBufferMemory);
        stbi_image_free(pixels);
        Image image{};
        image.width = width;
        image.height = height;
        image.stagingBuffer = { stagingBuffer, stagingBufferMemory };
        image.samplingMode = samplingMode;
        this->textures.emplace_back(image);
    }

    void Texture::createDescriptorPool(uint32_t frameCount) {
        VkDescriptorPoolSize poolSize{};
        poolSize.type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        poolSize.descriptorCount = frameCount;
        VkDescriptorPoolCreateInfo poolInfo{};
        poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
        poolInfo.poolSizeCount = 1;
        poolInfo.pPoolSizes = &poolSize;
        poolInfo.maxSets = frameCount;
        if (vkCreateDescriptorPool(this->_device->logicalDevice, &poolInfo, nullptr, &this->_descriptorPool) != VK_SUCCESS) {
            throw std::runtime_error("failed to create descriptor pool!");
        }
    }

    void Texture::createDescriptorSets(uint32_t frameCount) {
        std::vector<VkDescriptorSetLayout> layouts(frameCount, this->_descriptorSetLayout);
        VkDescriptorSetAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
        allocInfo.descriptorPool = this->_descriptorPool;
        allocInfo.descriptorSetCount = frameCount;
        allocInfo.pSetLayouts = layouts.data();
        this->_descriptorSets.resize(frameCount);
        if (vkAllocateDescriptorSets(this->_device->logicalDevice, &allocInfo, this->_descriptorSets.data()) != VK_SUCCESS) {
            throw std::runtime_error("failed to allocate descriptor sets!");
        }
        for (size_t i = 0; i < frameCount; i++) {
            VkDescriptorImageInfo   imageInfo{};
            imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
            // je suis ici
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


    void Texture::build(uint32_t frameCount) {
        this->transferTextureToGPU();
        this->createTexturesView();
        this->createDescriptorPool(frameCount);
        this->createDescriptorSets();
        this->createDescriptorLayout();
    }
}

