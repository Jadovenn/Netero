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

        void transferVertexBuffer();
        void release();
        void createGraphicsPipeline(VkRenderPass, VkExtent2D, VkDescriptorSetLayout);
        void commitRenderCommand(VkRenderPass, VkCommandBuffer, VkFramebuffer, VkDescriptorSet, VkExtent2D);

        VkInstance          _instance;
        Device*             _device;
        VertexBuffer        _vertexBuffer;
        VkPipelineLayout    _pipelineLayout;
        VkPipeline          _graphicsPipeline;
        std::vector<Shader>     _shaderModules;
        std::vector<Instance*>  _modelInstances;
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

