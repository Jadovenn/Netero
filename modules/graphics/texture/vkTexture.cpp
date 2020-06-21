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
        this->release();
        if (this->_stagingBuffer) {
            vkDestroyBuffer(this->_device->logicalDevice, this->_stagingBuffer, nullptr);
            vkFreeMemory(this->_device->logicalDevice, this->_stagingBufferMemory, nullptr);
        }
        if (this->_image) {
            vkDestroyImageView(this->_device->logicalDevice, this->_imageView, nullptr);
            vkDestroyImage(this->_device->logicalDevice, this->_image, nullptr);
            vkFreeMemory(this->_device->logicalDevice, this->_imageMemory, nullptr);
            vkDestroySampler(this->_device->logicalDevice, this->_textureSampler, nullptr);
        }
    }

    void Texture::release() {
        //vkDestroyDescriptorPool(this->_device->logicalDevice, this->_descriptorPool, nullptr);
    }

    void Texture::transferTextureToGPU() {
        VkCommandBuffer transferCmd = vkUtils::BeginCommandBufferRecording(this->_device->logicalDevice,
            this->_device->transferCommandPool,
            VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);

        auto [image, imageMemory] = vkUtils::AllocImage(this->_device,
            this->_width,
            this->_height,
            VK_FORMAT_R8G8B8A8_SRGB,
            VK_IMAGE_TILING_OPTIMAL,
            VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
            VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
        this->_image = image;
        this->_imageMemory = imageMemory;
        vkUtils::TransitionImageLayout(transferCmd,
            image,
            VK_FORMAT_R8G8B8A8_SRGB,
            VK_IMAGE_LAYOUT_UNDEFINED,
            VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
        vkUtils::TransferImage(transferCmd,
            this->_stagingBuffer,
            this->_image,
            this->_width,
            this->_height);
        vkUtils::TransitionImageLayout(transferCmd,
            this->_image,
            VK_FORMAT_R8G8B8A8_SRGB,
            VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
            VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

        vkUtils::FlushCommandBuffer(this->_device->logicalDevice,
            this->_device->transferQueue,
            this->_device->transferCommandPool,
            transferCmd);
        vkDestroyBuffer(this->_device->logicalDevice, this->_stagingBuffer, nullptr);
        vkFreeMemory(this->_device->logicalDevice, this->_stagingBufferMemory, nullptr);
        this->_stagingBuffer = nullptr;
        this->_stagingBufferMemory = nullptr;
    }

    void Texture::createTexturesView() {
        this->_imageView = vkUtils::CreateImageView(this->_device->logicalDevice,
            this->_image,
            VK_FORMAT_R8G8B8A8_SRGB);
    }

    void Texture::createTexturesSampler() {
        VkSamplerCreateInfo samplerInfo{};
        samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
        samplerInfo.magFilter = VK_FILTER_LINEAR;
        samplerInfo.minFilter = VK_FILTER_LINEAR;
        samplerInfo.addressModeU = static_cast<VkSamplerAddressMode>(this->_samplingMode);
        samplerInfo.addressModeV = static_cast<VkSamplerAddressMode>(this->_samplingMode);
        samplerInfo.addressModeW = static_cast<VkSamplerAddressMode>(this->_samplingMode);
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
        if (vkCreateSampler(this->_device->logicalDevice, &samplerInfo, nullptr, &this->_textureSampler) != VK_SUCCESS) {
            throw std::runtime_error("failed to create texture sampler!");
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

        this->_width = width;
        this->_height = height;
        this->_stagingBuffer = stagingBuffer;
        this->_stagingBufferMemory = stagingBufferMemory;
        this->_samplingMode = samplingMode;
    }

    void Texture::build(uint32_t frameCount) {
        if (this->empty()) { // There is no texture this time
            return;
        }
        this->transferTextureToGPU();
        this->createTexturesView();
        this->createTexturesSampler();
    }

    void Texture::rebuild(uint32_t frameCount) {
        if (this->empty()) {
            return;
        }
        this->release();
    }

    bool Texture::empty() const {
        return !this->_stagingBuffer && !this->_image;
    }

}

