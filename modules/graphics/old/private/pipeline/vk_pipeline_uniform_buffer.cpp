/**
 * Netero sources under BSD-3-Clause
 * see LICENSE.txt
 */

#define GLM_FORCE_DEPTH_ZERO_TO_ONE

#include <glm/gtc/matrix_transform.hpp>

#include <netero/graphics/context.hpp>
#include <netero/graphics/pipeline.hpp>

#include "utils/vkUtils.hpp"

namespace netero::graphics {

void Pipeline::createUniformBuffers()
{
    const size_t       swapchainImagesCount = this->swapchainImages.size();
    const VkDeviceSize size = sizeof(UniformBufferObject);
    this->_uniformBuffers.resize(swapchainImagesCount);
    this->_uniformBuffersMemory.resize(swapchainImagesCount);
    for (size_t idx = 0; idx < swapchainImagesCount; ++idx) {
        auto [buffer, bufferMemory] = vkUtils::AllocBuffer(
            this->_device,
            size,
            VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
        this->_uniformBuffers[idx] = buffer;
        this->_uniformBuffersMemory[idx] = bufferMemory;
    }
}

void Pipeline::update(uint32_t frameIndex)
{
    UniformBufferObject ubo {};
    ubo.view =
        glm::lookAt(glm::vec3(3.f, 0.f, 1.f), glm::vec3(0.f, 0.f, 0.f), glm::vec3(1.f, 0.f, 0.f));
    ubo.proj = glm::perspectiveRH(glm::radians(45.f),
                                  this->swapchainExtent.width /
                                      static_cast<float>(this->swapchainExtent.height),
                                  0.1f,
                                  100.f);
    UniformBufferObject* data = nullptr;
    vkMapMemory(this->_device->logicalDevice,
                this->_uniformBuffersMemory[frameIndex],
                0,
                sizeof(UniformBufferObject),
                0,
                reinterpret_cast<void**>(&data));
    std::memcpy(data, &ubo, sizeof(UniformBufferObject));
    vkUnmapMemory(this->_device->logicalDevice, this->_uniformBuffersMemory[frameIndex]);
}
} // namespace netero::graphics
