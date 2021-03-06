/**
 * Netero sources under BSD-3-Clause
 * see LICENSE.txt
 */

#include <fstream>
#include <stdexcept>

#include <netero/graphics/device.hpp>
#include <netero/graphics/instance.hpp>
#include <netero/graphics/model.hpp>

#include "utils/vkUtils.hpp"

static std::vector<char> ReadBinaryFile(const std::string& filePath)
{
    std::ifstream file(filePath, std::ios::ate | std::ios::binary);

    if (!file.is_open()) {
        throw std::runtime_error("Failed to open (" + filePath + ")");
    }
    const size_t      size = file.tellg();
    std::vector<char> buffer(size);
    file.seekg(0);
    file.read(buffer.data(), size);
    file.close();
    return buffer;
}

namespace netero::graphics {

Model::Model(VkInstance instance, Device* device)
    : _instance(instance),
      _device(device),
      _vertexBuffer(_device),
      _pipelineLayout(nullptr),
      _graphicsPipeline(nullptr),
      _instanceBuffer(nullptr),
      _instanceBufferMemory(nullptr),
      _textures(device),
      _descriptorSets(device)
{
}

Model::~Model()
{
    for (auto& shader : this->_shaderModules) {
        vkDestroyShaderModule(this->_device->logicalDevice, shader.shaderModule, nullptr);
    }
    for (auto* instance : _modelInstances) {
        delete instance;
    }
    this->_vertexBuffer.release();
    vkDestroyBuffer(this->_device->logicalDevice, this->_instanceBuffer, nullptr);
    vkFreeMemory(this->_device->logicalDevice, this->_instanceBufferMemory, nullptr);
}

void Model::release(size_t imageCount)
{
    if (this->_modelInstances.empty()) {
        return;
    }
    vkDestroyPipeline(this->_device->logicalDevice, this->_graphicsPipeline, nullptr);
    vkDestroyPipelineLayout(this->_device->logicalDevice, this->_pipelineLayout, nullptr);
}

void Model::build(size_t                 framesCount,
                  std::vector<VkBuffer>& uboBuffers,
                  VkRenderPass           renderPass,
                  VkExtent2D             extent)
{
    if (this->_modelInstances.empty()) {
        return;
    }
    this->_vertexBuffer.AllocateAndTransfer(this->_modelInstances.size());
    this->_textures.build(framesCount);
    this->createInstanceBuffer(framesCount);
    this->createDescriptors(framesCount);
    this->writeToDescriptorSet(framesCount, uboBuffers);
    this->createGraphicsPipeline(renderPass, extent);
}

void Model::rebuild(size_t                 imageCount,
                    std::vector<VkBuffer>& uboBuffers,
                    VkRenderPass           renderPass,
                    VkExtent2D             extent)
{
    if (this->_modelInstances.empty()) {
        return;
    }
    this->release(imageCount);
    this->_descriptorSets.rebuild();
    this->writeToDescriptorSet(imageCount, uboBuffers);
    this->createGraphicsPipeline(renderPass, extent);
}

Instance* Model::createInstance()
{
    auto* instance = new Instance();
    this->_modelInstances.push_back(instance);
    return instance;
}

void Model::deleteInstance(Instance* instance)
{
    auto it = std::find(this->_modelInstances.begin(), this->_modelInstances.end(), instance);
    if (it != this->_modelInstances.end()) {
        this->_modelInstances.erase(it);
        delete instance;
    }
}

void Model::addVertices(std::vector<Vertex>& vertices)
{
    const int v_size = this->_vertexBuffer.vertices.size();
    const int i_size = vertices.size();
    for (unsigned idx = v_size; idx < i_size + v_size; ++idx) {
        this->_vertexBuffer.indices.push_back(idx);
    }
    this->_vertexBuffer.vertices.insert(this->_vertexBuffer.vertices.end(),
                                        vertices.begin(),
                                        vertices.end());
}

void Model::addVertices(std::vector<Vertex>& vertices, std::vector<uint32_t>& indices)
{
    this->_vertexBuffer.vertices.insert(this->_vertexBuffer.vertices.end(),
                                        vertices.begin(),
                                        vertices.end());
    this->_vertexBuffer.indices.insert(this->_vertexBuffer.indices.end(),
                                       indices.begin(),
                                       indices.end());
}

void Model::loadTexture(const std::string& path, TextureSamplingMode samplingMode)
{
    this->_textures.loadTexture(path, samplingMode);
}

int Model::loadShader(const std::string& path, ShaderStage stage)
{
    Shader shader;
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
} // namespace netero::graphics
