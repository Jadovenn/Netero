/**
 * Netero sources under BSD-3-Clause
 * see LICENSE.txt
 */

#pragma once

#include <vector>
#include <vulkan/vulkan.h>
#include <netero/graphics/device.hpp>

namespace netero::graphics {

    /**
     * Descriptor resources.
     */
    struct Descriptor {
        VkDescriptorType    descriptorType = VK_DESCRIPTOR_TYPE_MAX_ENUM;
        VkShaderStageFlags  shaderStage = VK_SHADER_STAGE_FLAG_BITS_MAX_ENUM;
        uint32_t            binding = 0;
    };

    class DescriptorSets {
    public:

        static const int WHOLE_SIZE; /**< Index to specify an operation on the entire set. */

        /**
         * Return Type enumeration.
         */
        enum class Error {
            SUCCESS, /**< Returned if succeeded. */
            BAD_DESCRIPTOR_TYPE, /**< Returned if operation not supported for the given descriptor. */
            EMPTY_DESCRIPTOR_SET, /**< Returned if the descriptor set is empty. */
            INDEX_OUT_OF_BOUND /**< Returned if an provided index is out of bound. */
        };

        explicit DescriptorSets(Device* device, uint32_t descriptorSetCount = 0);
        DescriptorSets(const DescriptorSets&) = delete;
        DescriptorSets(DescriptorSets&&) = delete;
        DescriptorSets& operator=(const DescriptorSets&) = delete;
        DescriptorSets& operator=(DescriptorSets&&) = delete;
        ~DescriptorSets();


        /**
         * Set the number of descriptor per set.
         * @details This is usually the same as the number of frame in the swapchain.
         */
        void    setSetsCount(uint32_t);


        /**
         * Add a new descriptor to the set.
         */
        void    add(Descriptor&);

        /**
         * Return the layout for the given descriptor set.
         * @attention If called before build(), the result is nullptr.
         */
        [[nodiscard]] VkDescriptorSetLayout* getLayout();


        /**
         * Return the descriptor set at the specific index.
         * @attention If called before build(), the result is undefined.
         */
        [[nodiscard]] VkDescriptorSet* at(uint32_t);

        /**
         * Build the descriptor set.
         * @details Every attributes must be set before this call.
         */
        void    build();

        /**
         * Rebuild the necessary resources while the swapchain need to be rebuild.
         */
        void    rebuild();

        /**
         * Release the necessary resources while the swapchain need to be rebuild.
         */
        void    release();

        /**
         * Write a descriptor to a set of type combined image sampler
         * @param descriptor descriptor to write in
         * @param setIdx index of the descriptor set
         * @param imageView a valid VkImageView
         * @param sampler a valid VkSampler
         */
        Error    write(Descriptor& descriptor, int setIdx, VkImageView imageView, VkSampler sampler);

        /**
         * Write a descriptor to a set of type uniform buffer
         * @param descriptor descriptor to write in
         * @param setIdx index of the descriptor set
         * @param buffer a valid VkBuffer
         * @param offset in the given buffer
         * @param range to write from
         */
        Error    write(Descriptor& descriptor, int setIdx, VkBuffer buffer, VkDeviceSize offset, VkDeviceSize range);

    private:
        void    _createDescriptorSetPool();
        void    _createDescriptorSetLayout();
        void    _createDescriptorSets();
        Error   _write(Descriptor&, int, VkDescriptorBufferInfo*, VkDescriptorImageInfo*);

        // Instance Related
        Device*         _device = nullptr; /** Device handle. */
        uint32_t        _descriptorCount = 0; /**< Return the number of descriptor per set. */
        const uint32_t  _descriptorCountPerSet = 1; /**< Number of descriptor(s) of the same type in a set. Might be configurable in the future. */

        // Descriptor Related
        VkDescriptorPool                _descriptorPool = nullptr; /**< Descriptor pool.*/
        VkDescriptorSetLayout           _descriptorSetLayout = nullptr; /**< Descriptor set layout. */
        std::vector<Descriptor>         _descriptors; /**< Descriptors per set. */
        std::vector<VkDescriptorSet>    _descriptorSets; /** Descriptor sets. */
    };
}

