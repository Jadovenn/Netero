/**
 * Netero sources under BSD-3-Clause
 * see LICENSE.txt
 */

#pragma once

#include <string>
#include <vector>
#include <utility>
#include <vulkan/vulkan.h>
#include <netero/graphics/device.hpp>

namespace netero::graphics {

    enum class TextureSamplingMode {
        REPEAT = VK_SAMPLER_ADDRESS_MODE_REPEAT,
        MIRRORED_REPEAT = VK_SAMPLER_ADDRESS_MODE_MIRRORED_REPEAT,
        CLAMP_TO_EDGE = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE,
        MIRRORED_CLAMP_TO_EDGE = VK_SAMPLER_ADDRESS_MODE_MIRROR_CLAMP_TO_EDGE,
        CLAMP_TO_BORDER = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER,
    };

    struct Image {
        uint32_t width = 0;
        uint32_t height = 0;
        std::pair<VkBuffer, VkDeviceMemory> stagingBuffer = {nullptr, nullptr};
        std::pair<VkImage, VkDeviceMemory>  image = {nullptr, nullptr};
        VkImageView imageView;
        TextureSamplingMode samplingMode = TextureSamplingMode::REPEAT;
        VkSampler textureSampler = nullptr;
    };

    class Texture {
        void    transferTextureToGPU();
        void    createTexturesView();
        void    createTexturesSampler();
        void    createDescriptorPool(uint32_t);
        void    createDescriptorSets(uint32_t);
        void    createDescriptorLayout();

        Device* _device;

        VkDescriptorPool                _descriptorPool;
        VkDescriptorSetLayout           _descriptorSetLayout;
        std::vector<VkDescriptorSet>    _descriptorSets;
    public:
        Texture(Device*);
        ~Texture();

        void    loadTexture(const std::string&, TextureSamplingMode);
        void    build(uint32_t);

        std::vector<Image>  textures;
    };

}

