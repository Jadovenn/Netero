/**
 * Netero sources under BSD-3-Clause
 * see LICENSE.txt
 */

#include <stdexcept>
#include <fstream>
#include <netero/graphics/context.hpp>

#include "utils/vkUtils.hpp"

static std::vector<char>    ReadBinaryFile(const std::string& filePath) {
    std::ifstream   file(filePath, std::ios::ate | std::ios::binary);

    if (!file.is_open()) { throw std::runtime_error("Failed to open (" + filePath + ")"); }
    const size_t size = file.tellg();
    std::vector<char> buffer(size);
    file.seekg(0);
    file.read(buffer.data(), size);
    file.close();
    return buffer;
}

namespace netero::graphics {

    int Context::loadShader(const std::string& filePath, ShaderStage stage) {
        Shader  shader;
        shader.stage = stage;
        shader.byteCode = ReadBinaryFile(filePath);
        VkShaderModuleCreateInfo createInfo = {};
        createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
        createInfo.codeSize = shader.byteCode.size();
        createInfo.pCode = reinterpret_cast<const uint32_t*>(shader.byteCode.data());
        const VkResult result = vkCreateShaderModule(this->_device->logicalDevice,
            &createInfo,
            nullptr,
            &shader.shaderModule);
        if (result != VK_SUCCESS) {
            return 1;
        }
        this->_shaderModules.push_back(shader);
        return 0;
    }

    void Context::addVertices(std::vector<ColoredVertex> *vertices) {
        this->_vertices = vertices;
    }

    void Context::createVertexBuffer() {
        VkBufferCreateInfo  createInfo{};

        createInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        createInfo.size = sizeof(ColoredVertex) * this->_vertices->size();
        createInfo.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
        createInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
        createInfo.flags = 0;

        VkResult result = vkCreateBuffer(this->_device->logicalDevice,
            &createInfo,
            nullptr,
            &this->_vertexBuffer);
        if (result != VK_SUCCESS) {
            throw std::runtime_error("Error while creating vertex buffer.");
        }
        VkMemoryRequirements    memoryRequirements;
        vkGetBufferMemoryRequirements(this->_device->logicalDevice, this->_vertexBuffer, &memoryRequirements);
        const int index = vkUtils::FindMemoryType(this->_device->physicalDevice, memoryRequirements.memoryTypeBits, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
        if (index == -1) {
            throw std::runtime_error("Error could not find a suitable memory type for vertex buffer.");
        }
        VkMemoryAllocateInfo    allocateInfo{};
        allocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        allocateInfo.allocationSize = memoryRequirements.size;
        allocateInfo.memoryTypeIndex = index;
        result = vkAllocateMemory(this->_device->logicalDevice, &allocateInfo, nullptr, &this->_vertexBufferMemory);
        if (result != VK_SUCCESS) {
            throw std::runtime_error("Error could not allocate memory for vertex buffer");
        }
        vkBindBufferMemory(this->_device->logicalDevice, this->_vertexBuffer, this->_vertexBufferMemory, 0);
        void* data = nullptr;
        vkMapMemory(this->_device->logicalDevice, this->_vertexBufferMemory, 0, createInfo.size, 0, &data);
        std::memcpy(data, this->_vertices->data(), static_cast<size_t>(createInfo.size));
        vkUnmapMemory(this->_device->logicalDevice, this->_vertexBufferMemory);
    }
}

