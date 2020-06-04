/**
 * Netero sources under BSD-3-Clause
 * see LICENSE.txt
 */

#pragma once

#include <vector>
#include <vulkan/vulkan.h>
#include <netero/graphics/shader.hpp>
#include <netero/graphics/device.hpp>
#include <netero/graphics/vertex.hpp>
#include <netero/graphics/pipeline.hpp>

namespace netero::graphics {

    /**
     * @brief Actual instance of a model
     * An instance represent one model in the context
     * that is actually in the graphics pipeline.
     * Many instance of a model may exist.
     */
    class Instance {
    public:

    protected:
        friend class Context;
    };

    /**
     * @brief Any model to render
     * Contain assets and data to be rendered.
     * @details Only one model exist per instance. Models are used to
     * hold vertices, shaders, textures...
     */
    class Model {
        Model(VkInstance, Device*);
        friend class netero::graphics::Context;
        friend class netero::graphics::Pipeline;

        void build(size_t, VkRenderPass, VkExtent2D);
        void rebuild(size_t, VkRenderPass, VkExtent2D);
        void release(size_t);
        void transferVertexBuffer();

        void createUniformBuffers(size_t); // DONE
        void createDescriptorPool(size_t);  // DONE
        void createDescriptorSets(size_t); // DONE
        void createDescriptorSetLayout(); // DONE
        void createGraphicsPipeline(VkRenderPass, VkExtent2D);
        void commitRenderCommand(VkRenderPass, VkCommandBuffer, VkFramebuffer, VkExtent2D, size_t);
        void updateMVP(uint32_t, VkExtent2D);

        VkInstance          _instance;
        Device*             _device;
        VertexBuffer        _vertexBuffer;
        VkPipelineLayout    _pipelineLayout;
        VkPipeline          _graphicsPipeline;
        VkDescriptorPool    _descriptorPool;
        VkDescriptorSetLayout   _descriptorSetLayout;
        std::vector<Shader>     _shaderModules;
        std::vector<Instance*>  _modelInstances;
        std::vector<VkBuffer>           uniformBuffers;
        std::vector<VkDeviceMemory>     uniformBuffersMemory;
        std::vector<VkDescriptorSet>    _descriptorSets;
    public:
        ~Model();

        Instance* createInstance();
        void deleteInstance(Instance*);

        void    addVertices(std::vector<Vertex>&);
        void    addVertices(std::vector<Vertex>&, std::vector<uint16_t>&);
        int     loadShader(const std::string&, ShaderStage);
        void    loadTexture(const std::string&);

    };
}

