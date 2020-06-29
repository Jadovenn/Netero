/**
 * Netero sources under BSD-3-Clause
 * see LICENSE.txt
 */

#pragma once

#include <string>

#include <vulkan/vulkan.h>

namespace netero::graphics {

class Device;

enum class TextureSamplingMode {
    REPEAT = VK_SAMPLER_ADDRESS_MODE_REPEAT,
    MIRRORED_REPEAT = VK_SAMPLER_ADDRESS_MODE_MIRRORED_REPEAT,
    CLAMP_TO_EDGE = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE,
    MIRRORED_CLAMP_TO_EDGE = VK_SAMPLER_ADDRESS_MODE_MIRROR_CLAMP_TO_EDGE,
    CLAMP_TO_BORDER = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER,
};

class Texture {
    void transferTextureToGPU();
    void createTexturesView();
    void createTexturesSampler();

    Device* _device;

    uint32_t            _width = 0;
    uint32_t            _height = 0;
    VkBuffer            _stagingBuffer = nullptr;
    VkDeviceMemory      _stagingBufferMemory = nullptr;
    VkImage             _image = nullptr;
    VkDeviceMemory      _imageMemory = nullptr;
    VkImageView         _imageView = nullptr;
    VkSampler           _textureSampler = nullptr;
    TextureSamplingMode _samplingMode = TextureSamplingMode::REPEAT;

    public:
    Texture(Device*);
    ~Texture();

    void                      loadTexture(const std::string&, TextureSamplingMode);
    void                      build(uint32_t);
    void                      rebuild(uint32_t);
    void                      release();
    [[nodiscard]] bool        empty() const;
    [[nodiscard]] VkImageView getImageView() const { return this->_imageView; };
    [[nodiscard]] VkSampler   getSampler() const { return this->_textureSampler; }
};

} // namespace netero::graphics
