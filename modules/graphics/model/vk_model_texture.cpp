/**
 * Netero sources under BSD-3-Clause
 * see LICENSE.txt
 */

#include <stdexcept>
#include <netero/graphics/model.hpp>
#include <netero/extra/stb_image.h>

#include "utils/vkUtils.hpp"

namespace netero::graphics {

    void Model::loadTexture(const std::string& path) {
        int width, height, channels = 0;
        stbi_uc* pixels = stbi_load(path.c_str(), &width, &height, &channels, STBI_rgb_alpha);
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
        auto [image, imageMemory] = vkUtils::AllocImage(this->_device,
            width,
            height,
            VK_FORMAT_R8G8B8A8_SRGB,
            VK_IMAGE_TILING_OPTIMAL,
            VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
            VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
        this->_image = image;
        this->_imageMemory = imageMemory;
        vkUtils::TransitionImageLayout(this->_device,
            this->_image,
            VK_FORMAT_R8G8B8A8_SRGB,
            VK_IMAGE_LAYOUT_UNDEFINED,
            VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
        vkUtils::TransferImage(this->_device,
            stagingBuffer,
            this->_image,
            width,
            height);
        vkUtils::TransitionImageLayout(this->_device,
            this->_image,
            VK_FORMAT_R8G8B8A8_SRGB,
            VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
            VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
        vkDestroyBuffer(this->_device->logicalDevice, stagingBuffer, nullptr);
        vkFreeMemory(this->_device->logicalDevice, stagingBufferMemory, nullptr);
    }
}

