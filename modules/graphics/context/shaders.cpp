/**
 * Netero sources under BSD-3-Clause
 * see LICENSE.txt
 */

#include <stdexcept>
#include <fstream>
#include <netero/graphics/context.hpp>
#include <netero/graphics/vertex.hpp>

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

    void Context::addVertices(std::vector < netero::graphics::Vertex>& vertices) {
        const int v_size = this->_vertexBuffer->vertices.size();
        const int i_size = vertices.size();
        for (unsigned idx = v_size; idx < i_size + v_size; ++idx) {
            this->_vertexBuffer->indices.push_back(idx);
        }
        this->_vertexBuffer->vertices.insert(this->_vertexBuffer->vertices.end(), vertices.begin(), vertices.end());
    }

    void Context::addVertices(std::vector < netero::graphics::Vertex>& vertices, std::vector<uint16_t>& indices) {
        this->_vertexBuffer->vertices.insert(this->_vertexBuffer->vertices.end(), vertices.begin(), vertices.end());
        this->_vertexBuffer->indices.insert(this->_vertexBuffer->indices.end(), indices.begin(), indices.end());
    }
}

