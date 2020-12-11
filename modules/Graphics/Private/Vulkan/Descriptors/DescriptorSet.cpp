/**
 * Netero sources under BSD-3-Clause
 * see LICENSE.txt
 */

#include "Vulkan/Descriptors/DescriptorSet.hpp"

#include "Vulkan/Context/Context.hpp"

namespace Netero::Gfx {

DescriptorSets::DescriptorSets(Context& aContext, uint32_t aDescriptorSetCount)
    : myContext(aContext), myDescriptorCount(aDescriptorSetCount)
{
    assert(myContext.myLogicalDevice);
}

DescriptorSets::~DescriptorSets()
{
    this->Release();
    vkDestroyDescriptorSetLayout(myContext.myLogicalDevice, myDescriptorSetLayout, nullptr);
}

void DescriptorSets::SetSetsCount(uint32_t descriptorSetCount)
{
    myDescriptorCount = descriptorSetCount;
}

void DescriptorSets::Add(Descriptor& descriptor)
{
    myDescriptors.push_back(descriptor);
}

void DescriptorSets::Build()
{
    myDescriptors.shrink_to_fit();
    this->_createDescriptorSetLayout();
    this->_createDescriptorSetPool();
    this->_createDescriptorSets();
}

void DescriptorSets::Rebuild()
{
    this->Release();
    this->_createDescriptorSetPool();
    this->_createDescriptorSets();
}

VkDescriptorSetLayout* DescriptorSets::GetLayout()
{
    return &myDescriptorSetLayout;
}

VkDescriptorSet* DescriptorSets::At(uint32_t index)
{
    if (index >= myDescriptorSets.size()) {
        return nullptr;
    }
    return &myDescriptorSets[index];
}

void DescriptorSets::Release()
{
    vkDestroyDescriptorPool(myContext.myLogicalDevice, myDescriptorPool, nullptr);
}

void DescriptorSets::_createDescriptorSetPool()
{
    std::vector<VkDescriptorPoolSize> poolSizes;
    for (auto& desc : myDescriptors) {
        VkDescriptorPoolSize poolSize {};
        poolSize.type = desc.myDescriptorType;
        poolSize.descriptorCount = myDescriptorCount;
        poolSizes.push_back(poolSize);
    }
    VkDescriptorPoolCreateInfo poolInfo {};
    poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    poolInfo.poolSizeCount = poolSizes.size();
    poolInfo.pPoolSizes = poolSizes.data();
    poolInfo.maxSets = myDescriptorCount;
    if (vkCreateDescriptorPool(myContext.myLogicalDevice, &poolInfo, nullptr, &myDescriptorPool) !=
        VK_SUCCESS) {
        throw std::runtime_error("failed to create descriptor pool!");
    }
}

void DescriptorSets::_createDescriptorSetLayout()
{
    std::vector<VkDescriptorSetLayoutBinding> bindings;

    for (auto& desc : myDescriptors) {
        VkDescriptorSetLayoutBinding binding {};
        binding.binding = desc.myBinding;
        binding.descriptorCount = myDescriptorCountPerSet;
        binding.descriptorType = desc.myDescriptorType;
        binding.stageFlags = desc.myShaderStage;
        binding.pImmutableSamplers = nullptr;
        bindings.push_back(binding);
    }

    VkDescriptorSetLayoutCreateInfo layoutInfo {};
    layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    layoutInfo.bindingCount = bindings.size();
    layoutInfo.pBindings = bindings.data();

    const VkResult result = vkCreateDescriptorSetLayout(myContext.myLogicalDevice,
                                                        &layoutInfo,
                                                        nullptr,
                                                        &myDescriptorSetLayout);
    if (result != VK_SUCCESS) {
        throw std::runtime_error("Failed to create descriptor set layout.");
    }
}

void DescriptorSets::_createDescriptorSets()
{
    std::vector<VkDescriptorSetLayout> layouts(myDescriptorCount, myDescriptorSetLayout);
    VkDescriptorSetAllocateInfo        allocInfo {};
    allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    allocInfo.descriptorPool = myDescriptorPool;
    allocInfo.descriptorSetCount = myDescriptorCount;
    allocInfo.pSetLayouts = layouts.data();
    myDescriptorSets.resize(myDescriptorCount);
    if (vkAllocateDescriptorSets(myContext.myLogicalDevice, &allocInfo, myDescriptorSets.data()) !=
        VK_SUCCESS) {
        throw std::runtime_error("failed to allocate descriptor sets!");
    }
}

GfxResult
DescriptorSets::Write(Descriptor& descriptor, int setIdx, VkImageView imageView, VkSampler sampler)
{
    if (myDescriptorSets.empty()) {
        return GfxResult::EMPTY_DESCRIPTOR_SET;
    }
    if (descriptor.myDescriptorType != VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER) {
        return GfxResult::BAD_DESCRIPTOR_TYPE;
    }
    if (setIdx != WHOLE_SIZE && setIdx < 0 && setIdx >= static_cast<int>(myDescriptorSets.size())) {
        return GfxResult::INDEX_OUT_OF_BOUND;
    }
    VkDescriptorImageInfo imageInfo;
    imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    imageInfo.imageView = imageView;
    imageInfo.sampler = sampler;
    this->_write(descriptor, setIdx, nullptr, &imageInfo);
    return GfxResult::SUCCESS;
}

GfxResult DescriptorSets::Write(Descriptor&  descriptor,
                                int          setIdx,
                                VkBuffer     buffer,
                                VkDeviceSize offset,
                                VkDeviceSize range)
{
    if (myDescriptorSets.empty()) {
        return GfxResult::EMPTY_DESCRIPTOR_SET;
    }
    if (descriptor.myDescriptorType != VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER) {
        return GfxResult::BAD_DESCRIPTOR_TYPE;
    }
    if (setIdx != WHOLE_SIZE && setIdx < 0 && setIdx >= static_cast<int>(myDescriptorSets.size())) {
        return GfxResult::INDEX_OUT_OF_BOUND;
    }
    VkDescriptorBufferInfo bufferInfo;
    bufferInfo.buffer = buffer;
    bufferInfo.offset = offset;
    bufferInfo.range = range;
    this->_write(descriptor, setIdx, &bufferInfo, nullptr);
    return GfxResult::SUCCESS;
}

GfxResult DescriptorSets::_write(Descriptor&             descriptor,
                                 int                     setIdx,
                                 VkDescriptorBufferInfo* bufferInfo,
                                 VkDescriptorImageInfo*  imageInfo)
{
    if (setIdx == WHOLE_SIZE) {
        for (size_t i = 0; i < myDescriptorCount; i++) {
            VkWriteDescriptorSet descriptorWrite {};
            descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            descriptorWrite.dstSet = myDescriptorSets[i];
            descriptorWrite.dstArrayElement = 0;
            descriptorWrite.dstBinding = descriptor.myBinding;
            descriptorWrite.descriptorType = descriptor.myDescriptorType;
            descriptorWrite.descriptorCount = myDescriptorCountPerSet;
            descriptorWrite.pBufferInfo = bufferInfo ? bufferInfo : nullptr;
            descriptorWrite.pImageInfo = imageInfo ? imageInfo : nullptr;
            descriptorWrite.pTexelBufferView = nullptr;
            vkUpdateDescriptorSets(myContext.myLogicalDevice, 1, &descriptorWrite, 0, nullptr);
        }
    }
    else {
        VkWriteDescriptorSet descriptorWrite {};
        descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        descriptorWrite.dstSet = myDescriptorSets[setIdx];
        descriptorWrite.dstBinding = descriptor.myBinding;
        descriptorWrite.dstArrayElement = 0;
        descriptorWrite.descriptorType = descriptor.myDescriptorType;
        descriptorWrite.descriptorCount = myDescriptorCountPerSet;
        descriptorWrite.pBufferInfo = bufferInfo ? bufferInfo : nullptr;
        descriptorWrite.pImageInfo = imageInfo ? imageInfo : nullptr;
        descriptorWrite.pTexelBufferView = nullptr;
        vkUpdateDescriptorSets(myContext.myLogicalDevice, 1, &descriptorWrite, 0, nullptr);
    }
    return GfxResult::SUCCESS;
}

} // namespace Netero::Gfx
