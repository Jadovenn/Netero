/**
 * Netero sources under BSD-3-Clause
 * see LICENSE.txt
 */

#pragma once

#include <vector>
#include <vulkan/vulkan.h>
#include <netero/graphics/device.hpp>

namespace netero::graphics {

    struct Descriptor {
        VkDescriptorType    descriptorType = VK_DESCRIPTOR_TYPE_MAX_ENUM;
        VkShaderStageFlags  shaderStage = VK_SHADER_STAGE_FLAG_BITS_MAX_ENUM;
        uint32_t            binding = 0;
    };

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
        void    add(Descriptor&);
        [[nodiscard]] VkDescriptorSetLayout* getLayout();
        [[nodiscard]] VkDescriptorSet* at(uint32_t);

        void    build();
        void    rebuild();
        void    release();

        Error    write(Descriptor&, int, VkImageView, VkSampler);
        Error    write(Descriptor&, int, VkBuffer, VkDeviceSize, VkDeviceSize);

    private:
        void    _createDescriptorSetPool();
        void    _createDescriptorSetLayout();
        void    _createDescriptorSets();
        Error   _write(Descriptor&, int, VkDescriptorBufferInfo*, VkDescriptorImageInfo*);

        // Instance Related
        Device*         _device = nullptr;
        uint32_t        _descriptorCount = 0;
        const uint32_t  _descriptorCountPerSet = 1; /**< Number of descriptor(s) in a set. Might be configurable in the future. */

        // Descriptor Related
        std::vector<Descriptor>         _descriptors;
        VkDescriptorPool                _descriptorPool = nullptr;
        VkDescriptorSetLayout           _descriptorSetLayout = nullptr;
        std::vector<VkDescriptorSet>    _descriptorSets;
    };
}

