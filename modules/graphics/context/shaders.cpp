/**
 * Netero sources under BSD-3-Clause
 * see LICENSE.txt
 */

#include <stdexcept>
#include <fstream>
#include <netero/graphics/context.hpp>

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
        const VkResult result = vkCreateShaderModule(this->_logicalDevice,
            &createInfo,
            nullptr,
            &shader.shaderModule);
        if (result != VK_SUCCESS) {
            return 1;
        }
        this->_shaderModules.push_back(shader);
        return 0;
    }

}

