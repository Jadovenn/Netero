/**
 * Netero sources under BSD-3-Clause
 * see LICENSE.txt
 */

#pragma once

#include <vector>

#include <Netero/Graphics/Errors.hpp>

#include "Vulkan/Descriptors/Descriptor.hpp"
#include "Vulkan/Vulkan.hpp"

namespace Netero::Gfx {

class Context;

class DescriptorSets {
    public:
    static const int WHOLE_SIZE; /**< Index to specify an operation on the entire set. */

    explicit DescriptorSets(Context& device, uint32_t descriptorSetCount = 0);
    DescriptorSets(const DescriptorSets&) = delete;
    DescriptorSets(DescriptorSets&&) = delete;
    DescriptorSets& operator=(const DescriptorSets&) = delete;
    DescriptorSets& operator=(DescriptorSets&&) = delete;
    ~DescriptorSets();

    /**
     * Set the number of descriptor per set.
     * @details This is usually the same as the number of frame in the swapchain.
     */
    void SetSetsCount(uint32_t);

    /**
     * Add a new descriptor to the set.
     */
    void Add(Descriptor&);

    /**
     * Return the layout for the given descriptor set.
     * @attention If called before build(), the result is nullptr.
     */
    [[nodiscard]] VkDescriptorSetLayout* GetLayout();

    /**
     * Return the descriptor set at the specific index.
     * @attention If called before build(), the result is undefined.
     */
    [[nodiscard]] VkDescriptorSet* At(uint32_t);

    /**
     * Build the descriptor set.
     * @details Every attributes must be set before this call.
     */
    void Build();

    /**
     * Rebuild the necessary resources while the swapchain need to be rebuild.
     */
    void Rebuild();

    /**
     * Release the necessary resources while the swapchain need to be rebuild.
     */
    void Release();

    /**
     * Write a descriptor to a set of type combined image sampler
     * @param descriptor descriptor to write in
     * @param setIdx index of the descriptor set
     * @param imageView a valid VkImageView
     * @param sampler a valid VkSampler
     */
    GfxResult Write(Descriptor& descriptor, int setIdx, VkImageView imageView, VkSampler sampler);

    /**
     * Write a descriptor to a set of type uniform buffer
     * @param descriptor descriptor to write in
     * @param setIdx index of the descriptor set
     * @param buffer a valid VkBuffer
     * @param offset in the given buffer
     * @param range to write from
     */
    GfxResult Write(Descriptor&  descriptor,
                    int          setIdx,
                    VkBuffer     buffer,
                    VkDeviceSize offset,
                    VkDeviceSize range);

    private:
    void      _createDescriptorSetPool();
    void      _createDescriptorSetLayout();
    void      _createDescriptorSets();
    GfxResult _write(Descriptor&, int, VkDescriptorBufferInfo*, VkDescriptorImageInfo*);

    // Instance Related
    Context&       myContext;
    uint32_t       myDescriptorCount = 0;
    const uint32_t myDescriptorCountPerSet = 1;

    // Descriptor Related
    VkDescriptorPool             myDescriptorPool = nullptr;      /**< Descriptor pool.*/
    VkDescriptorSetLayout        myDescriptorSetLayout = nullptr; /**< Descriptor set layout. */
    std::vector<Descriptor>      myDescriptors;                   /**< Descriptors per set. */
    std::vector<VkDescriptorSet> myDescriptorSets;
    /** Descriptor sets. */
};

} // namespace Netero::Gfx
