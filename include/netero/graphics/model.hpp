/**
 * Netero sources under BSD-3-Clause
 * see LICENSE.txt
 */

#pragma once

#include <vector>
#include <atomic>
#include <vulkan/vulkan.h>
#include <netero/graphics/shader.hpp>
#include <netero/graphics/device.hpp>
#include <netero/graphics/vertex.hpp>
#include <netero/graphics/instance.hpp>

namespace netero::graphics {

    /**
     * @brief Any model to render
     * Contain assets and data to be rendered.
     * @details Only one model exist per instance. Models are used to
     * hold vertices, shaders, textures...
     */
    class Model {
        friend class Pipeline;
        friend class Context;
        Model(VkInstance, Device*);

        void build(size_t, VkRenderPass, VkDescriptorSetLayout, VkExtent2D);
        void rebuild(size_t, VkRenderPass, VkDescriptorSetLayout, VkExtent2D);
        void release(size_t);

        void createInstanceBuffer(size_t);
        void createGraphicsPipeline(VkRenderPass, VkExtent2D, VkDescriptorSetLayout);
        void commitRenderCommand(VkCommandBuffer, VkDescriptorSet, size_t);
        void update(uint32_t);

        VkInstance          _instance;
        Device*             _device;
        VertexBuffer        _vertexBuffer;
        VkPipelineLayout    _pipelineLayout;
        VkPipeline          _graphicsPipeline;
        std::vector<Shader>     _shaderModules;
        std::vector<Instance*>  _modelInstances;

        // Model Buffer
        VkBuffer            instanceBuffer;
        VkDeviceMemory      instanceBufferMemory;
    public:
        Model(const Model&) = delete;
        Model(Model&&) = delete;
        Model& operator=(const Model&) = delete;
        Model& operator=(Model&&) = delete;
        ~Model();

        Instance* createInstance();
        void    deleteInstance(Instance*);
        void    addVertices(std::vector<Vertex>&);
        void    addVertices(std::vector<Vertex>&, std::vector<uint16_t>&);
        int     loadShader(const std::string&, ShaderStage);
        void    loadTexture(const std::string&);
    };
}

