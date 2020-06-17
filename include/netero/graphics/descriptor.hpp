/**
 * Netero sources under BSD-3-Clause
 * see LICENSE.txt
 */

#pragma once

#include <vector>
#include <vulkan/vulkan.h>
#include <netero/graphics/device.hpp>

namespace netero::graphics {

    class Descriptor {
    public:
        Descriptor(Device*, uint32_t);
        ~Descriptor();

        void    setDescriptorType(VkDescriptorType);
        void    setShaderStage(VkShaderStageFlags);
        void    setBinding(uint32_t);

        void    build();
        void    rebuild();
        void    release();
        void    update();

    private:
        void    _createDescriptorPool();
        void    _createDescriptorLayout();
        void    _createDescriptorSets();

        // Node Related
        Device*     _device = nullptr;
        uint32_t    _frameCount = 0;
        VkDescriptorType    _descriptorType = VK_DESCRIPTOR_TYPE_MAX_ENUM;
        VkShaderStageFlags  _shaderStage = VK_SHADER_STAGE_FLAG_BITS_MAX_ENUM;
        uint32_t            _binding = 0;

        // Descriptor Related
        VkDescriptorPool        _descriptorPool = nullptr;
        VkDescriptorSetLayout   _descriptorSetLayout = nullptr;
        std::vector<VkDescriptorSet>    _descriptorSets;
    };

}

