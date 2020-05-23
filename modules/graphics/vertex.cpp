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

    void VertexBuffer::createVertexBuffer() {
        VkBufferCreateInfo  createInfo{};

        createInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        createInfo.size = sizeof(Vertex) * this->vertices.size();
        createInfo.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
        createInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
        createInfo.flags = 0;

        VkResult result = vkCreateBuffer(this->_device->logicalDevice,
            &createInfo,
            nullptr,
            &this->vertexBuffer);
        if (result != VK_SUCCESS) {
            throw std::runtime_error("Error while creating vertex buffer.");
        }
        VkMemoryRequirements    memoryRequirements;
        vkGetBufferMemoryRequirements(this->_device->logicalDevice,
            this->vertexBuffer,
            &memoryRequirements);
        const int index = vkUtils::FindMemoryType(this->_device->physicalDevice, memoryRequirements.memoryTypeBits, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
        if (index == -1) {
            throw std::runtime_error("Error could not find a suitable memory type for vertex buffer.");
        }
        VkMemoryAllocateInfo    allocateInfo{};
        allocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        allocateInfo.allocationSize = memoryRequirements.size;
        allocateInfo.memoryTypeIndex = index;
        result = vkAllocateMemory(this->_device->logicalDevice,
            &allocateInfo,
            nullptr,
            &this->vertexBufferMemory);
        if (result != VK_SUCCESS) {
            throw std::runtime_error("Error could not allocate memory for vertex buffer");
        }
        vkBindBufferMemory(this->_device->logicalDevice,
            this->vertexBuffer,
            this->vertexBufferMemory,
            0);
        void* data = nullptr;
        vkMapMemory(this->_device->logicalDevice,
            this->vertexBufferMemory,
            0,
            createInfo.size,
            0,
            &data);
        std::memcpy(data,
            this->vertices.data(),
            static_cast<size_t>(createInfo.size));
        vkUnmapMemory(this->_device->logicalDevice,
            this->vertexBufferMemory);
    }

}

