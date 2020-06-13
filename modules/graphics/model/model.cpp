/**
 * Netero sources under BSD-3-Clause
 * see LICENSE.txt
 */

#include <stdexcept>
#include <fstream>
#include <netero/graphics/model.hpp>
#include <netero/extra/stb_image.h>

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

    Model::Model(VkInstance instance, Device* device)
        :   _instance(instance),
            _device(device),
            _vertexBuffer(_device),
            _pipelineLayout(nullptr),
            _graphicsPipeline(nullptr)
    {}

    Model::~Model() {
        for (auto& shader: this->_shaderModules) {
            vkDestroyShaderModule(this->_device->logicalDevice, shader.shaderModule, nullptr);
        }
        for (auto* instance: _modelInstances) {
            delete instance;
        }
        this->_vertexBuffer.release();
        vkDestroyBuffer(this->_device->logicalDevice, this->instanceBuffer, nullptr);
        vkFreeMemory(this->_device->logicalDevice, this->instanceBufferMemory, nullptr);
    }
    
    void Model::release(size_t imageCount) {
        if (this->_modelInstances.empty()) { return; }
        vkDestroyPipeline(this->_device->logicalDevice, this->_graphicsPipeline, nullptr);
        vkDestroyPipelineLayout(this->_device->logicalDevice, this->_pipelineLayout, nullptr);
    }

    void Model::build(size_t framesCount, VkRenderPass renderPass, VkDescriptorSetLayout descriptorSetLayout, VkExtent2D extent) {
        if (this->_modelInstances.empty()) { return; }
        this->_vertexBuffer.AllocateAndTransfer(this->_modelInstances.size());
        this->createInstanceBuffer(framesCount);
        this->createGraphicsPipeline(renderPass, extent, descriptorSetLayout);
    }

    void Model::rebuild(size_t imageCount, VkRenderPass renderPass, VkDescriptorSetLayout descriptorSetLayout, VkExtent2D extent) {
        if (this->_modelInstances.empty()) { return; }
        this->release(imageCount);
        this->createGraphicsPipeline(renderPass, extent, descriptorSetLayout);
    }

    Instance* Model::createInstance() {
        auto* instance = new Instance();
        this->_modelInstances.push_back(instance);
        return instance;
    }

    void Model::deleteInstance(Instance* instance) {
        auto it = std::find(this->_modelInstances.begin(), this->_modelInstances.end(), instance);
        if (it != this->_modelInstances.end()) {
            this->_modelInstances.erase(it);
            delete instance;
        }
    }

    void Model::addVertices(std::vector<Vertex>& vertices) {
        const int v_size = this->_vertexBuffer.vertices.size();
        const int i_size = vertices.size();
        for (unsigned idx = v_size; idx < i_size + v_size; ++idx) {
            this->_vertexBuffer.indices.push_back(idx);
        }
        this->_vertexBuffer.vertices.insert(this->_vertexBuffer.vertices.end(), vertices.begin(), vertices.end());
    }

    void Model::addVertices(std::vector<Vertex>& vertices, std::vector<uint16_t>& indices) {
        this->_vertexBuffer.vertices.insert(this->_vertexBuffer.vertices.end(), vertices.begin(), vertices.end());
        this->_vertexBuffer.indices.insert(this->_vertexBuffer.indices.end(), indices.begin(), indices.end());
    }

    int Model::loadShader(const std::string& path, ShaderStage stage) {
        Shader  shader;
        shader.stage = stage;
        shader.byteCode = ReadBinaryFile(path);
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

    void Model::loadTexture(const std::string& path) {
        int width, height, channels = 0;
        stbi_uc* pixels = stbi_load(path.c_str(), &width, &height, &channels, STBI_rgb_alpha);
        const VkDeviceSize imageSize = width * height * 4;
        if (!pixels) {
            throw std::runtime_error("Failed to load texture from fs.");
        }
        auto [stagingBuffer, stagingBufferMemory] = vkUtils::AllocBuffer(this->_device,
            imageSize,
            VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
        void* data;
        vkMapMemory(this->_device->logicalDevice,
            stagingBufferMemory,
            0,
            imageSize,
            0,
            &data);
        memcpy(data, pixels, static_cast<size_t>(imageSize));
        vkUnmapMemory(this->_device->logicalDevice, stagingBufferMemory);
        stbi_image_free(pixels);
    }

}

