/**
 * Netero sources under BSD-3-Clause
 * see LICENSE.txt
 */

#include <cassert>

#include <netero/graphics/vertex.hpp>

#include "utils/vkUtils.hpp"

namespace netero::graphics {

VertexBuffer::VertexBuffer(Device* device)
    : _device(device),
      vertexBuffer(nullptr),
      vertexBufferMemory(nullptr),
      indexBuffer(nullptr),
      indexBufferMemory(nullptr)
{
    assert(_device);
}

VertexBuffer::~VertexBuffer()
{
    if (this->vertexBuffer) {
        release();
    }
}

void VertexBuffer::release()
{
    vkDestroyBuffer(this->_device->logicalDevice, this->vertexBuffer, nullptr);
    vkFreeMemory(this->_device->logicalDevice, this->vertexBufferMemory, nullptr);
    vkDestroyBuffer(this->_device->logicalDevice, this->indexBuffer, nullptr);
    vkFreeMemory(this->_device->logicalDevice, this->indexBufferMemory, nullptr);
    this->vertexBuffer = nullptr;
    this->vertexBufferMemory = nullptr;
    this->indexBuffer = nullptr;
    this->indexBufferMemory = nullptr;
}

void VertexBuffer::AllocateAndTransfer(int instanceCount)
{
    if (this->vertexBuffer) {
        return;
    }
    this->createVertexBuffer();
    this->createIndexBuffer(instanceCount);
}

void VertexBuffer::createVertexBuffer()
{
    const VkDeviceSize size = sizeof(Vertex) * vertices.size();
    auto [stagingBuffer, stagingBufferMemory] = vkUtils::AllocBuffer(
        this->_device,
        size,
        VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
    void* data = nullptr;
    vkMapMemory(this->_device->logicalDevice, stagingBufferMemory, 0, size, 0, &data);
    std::memcpy(data, this->vertices.data(), static_cast<size_t>(size));
    vkUnmapMemory(this->_device->logicalDevice, stagingBufferMemory);
    auto [buffer, bufferMemory] = vkUtils::AllocBuffer(
        this->_device,
        size,
        VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
    vkUtils::TransferBuffer(this->_device, stagingBuffer, buffer, size);
    vkDestroyBuffer(this->_device->logicalDevice, stagingBuffer, nullptr);
    vkFreeMemory(this->_device->logicalDevice, stagingBufferMemory, nullptr);
    this->vertexBuffer = buffer;
    this->vertexBufferMemory = bufferMemory;
}

void VertexBuffer::createIndexBuffer(int instanceCount)
{
    const size_t       vertices_size = sizeof(uint32_t) * indices.size();
    const VkDeviceSize size = sizeof(uint32_t) * indices.size() * instanceCount;
    auto [stagingBuffer, stagingBufferMemory] = vkUtils::AllocBuffer(
        this->_device,
        size,
        VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
    void* data = nullptr;
    vkMapMemory(this->_device->logicalDevice, stagingBufferMemory, 0, size, 0, &data);
    for (int idx = 0; idx < instanceCount; ++idx) {
        std::memcpy(static_cast<char*>(data) + (idx * vertices_size),
                    this->indices.data(),
                    static_cast<size_t>(vertices_size));
    }
    vkUnmapMemory(this->_device->logicalDevice, stagingBufferMemory);
    auto [buffer, bufferMemory] = vkUtils::AllocBuffer(
        this->_device,
        size,
        VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
    vkUtils::TransferBuffer(this->_device, stagingBuffer, buffer, size);
    vkDestroyBuffer(this->_device->logicalDevice, stagingBuffer, nullptr);
    vkFreeMemory(this->_device->logicalDevice, stagingBufferMemory, nullptr);
    this->indexBuffer = buffer;
    this->indexBufferMemory = bufferMemory;
}
} // namespace netero::graphics
