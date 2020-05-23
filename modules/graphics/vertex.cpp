/**
 * Netero sources under BSD-3-Clause
 * see LICENSE.txt
 */

#include <cassert>
#include <netero/graphics/vertex.hpp>

#include "utils/vkUtils.hpp"

namespace netero::graphics {

    VertexBuffer::VertexBuffer(Device* device)
        :   _device(device),
            vertexBuffer(nullptr),
            vertexBufferMemory(nullptr)
    {
        assert(_device);
    }

    VertexBuffer::~VertexBuffer() {
        vkDestroyBuffer(this->_device->logicalDevice, this->vertexBuffer, nullptr);
        vkFreeMemory(this->_device->logicalDevice, this->vertexBufferMemory, nullptr);
    }

    // not supposed to actually call vkAllocateMemory for every individual buffer. Use one big buffer and offsets
    void VertexBuffer::createVertexBuffer() {
        const VkDeviceSize size = sizeof(Vertex) * vertices.size();
        auto [ stagingBuffer, stagingBufferMemory ] = vkUtils::AllocBuffer(this->_device,
            size,
            VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
        void* data = nullptr;
        vkMapMemory(this->_device->logicalDevice,
            stagingBufferMemory,
            0,
            size,
            0,
            &data);
        std::memcpy(data,
            this->vertices.data(),
            static_cast<size_t>(size));
        vkUnmapMemory(this->_device->logicalDevice,
            stagingBufferMemory);
        auto [ buffer, bufferMemory ] = vkUtils::AllocBuffer(this->_device,
            size,
            VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
        vkUtils::TransferBuffer(this->_device, stagingBuffer, buffer, size);
        vkDestroyBuffer(this->_device->logicalDevice, stagingBuffer, nullptr);
        vkFreeMemory(this->_device->logicalDevice, stagingBufferMemory, nullptr);
        this->vertexBuffer = buffer;
        this->vertexBufferMemory = bufferMemory;
    }

}

