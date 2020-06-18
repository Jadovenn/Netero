/**
 * Netero sources under BSD-3-Clause
 * see LICENSE.txt
 */

#pragma once

#include <vector>
#include <vulkan/vulkan.h>
#include <netero/graphics/device.hpp>

namespace netero::graphics {

    class DescriptorSets {
    public:

        static const int WHOLE_SIZE;

        enum class Error {
            SUCCESS,
            BAD_DESCRIPTOR_TYPE,
            EMPTY_DESCRIPTOR_SET,
            INDEX_OUT_OF_BOUND
        };

        explicit DescriptorSets(Device* device, uint32_t descriptorSetCount = 0);
        ~DescriptorSets();

        void    setSetsCount(uint32_t);
        void    setDescriptorSetType(VkDescriptorType);
        void    setShaderStage(VkShaderStageFlags);
        void    setBinding(uint32_t);
        [[nodiscard]] VkDescriptorSetLayout getLayout() const;
        [[nodiscard]] VkDescriptorSet at(uint32_t) const;

        void    build();
        void    rebuild();
        void    release();

        Error    write(int, VkImageView, VkSampler);
        Error    write(int, VkBuffer, VkDeviceSize, VkDeviceSize);

    private:
        void    _createDescriptorSetPool();
        void    _createDescriptorSetLayout();
        void    _createDescriptorSets();

        Error   _write(int, VkDescriptorBufferInfo*, VkDescriptorImageInfo*);

        // Instance Related
        Device*     _device = nullptr;
        uint32_t    _descriptorCount = 0;
        const uint32_t    _descriptorCountPerSet = 1; /**< Number of descriptor(s) in a set. Might be configurable in the future. */
        VkDescriptorType    _descriptorType = VK_DESCRIPTOR_TYPE_MAX_ENUM;
        VkShaderStageFlags  _shaderStage = VK_SHADER_STAGE_FLAG_BITS_MAX_ENUM;
        uint32_t            _binding = 0;

        // Descriptor Related
        VkDescriptorPool        _descriptorPool = nullptr;
        VkDescriptorSetLayout   _descriptorSetLayout = nullptr;
        std::vector<VkDescriptorSet>    _descriptorSets;
    };

}

