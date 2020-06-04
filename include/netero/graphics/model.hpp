/**
 * Netero sources under BSD-3-Clause
 * see LICENSE.txt
 */

#pragma once

#include <vector>
#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vulkan/vulkan.h>
#include <netero/graphics/shader.hpp>
#include <netero/graphics/device.hpp>
#include <netero/graphics/vertex.hpp>

namespace netero::graphics {

    // see: https://www.khronos.org/registry/vulkan/specs/1.1-extensions/html/chap14.html#interfaces-resources-layout
    // for alignment requirements
    struct UniformBufferObject {
        glm::mat4   model;
        glm::mat4   view;
        glm::mat4   proj;
    };

    /**
     * @brief Manage an Axis
     * This will in the feature be able to be bind
     * to an axis of a controller. For instance it is juste
     * an accessor to a float value.
     */
    class Axis {
        volatile float   _value = 0;
        volatile float   _rotation = 0;
        volatile float   _scale = 0;
    public:
        operator float() { return this->_value; }
        float operator=(float value) { this->_value = value; return this->_value; };

        void rotate(float value) { this->_rotation = value; }
        float getRotation() { return this->_rotation; }
        void scale(float value) { this->_scale = value; }
        float getScaling() { return this->_scale; }
    };

    /**
     * @brief Actual instance of a model
     * An instance represent one model in the context
     * that is actually in the graphics pipeline.
     * Many instance of a model may exist.
     */
    class Instance {
        friend class Context;

        // Need to recheck the order of the operation
        glm::mat4 getModelMat() {
            glm::mat4 model(1.f);
            model = glm::rotate(model, x.getRotation(), glm::vec3(1, 0, 0));
            model = glm::rotate(model, y.getRotation(), glm::vec3(0, 1, 0));
            model = glm::rotate(model, z.getRotation(), glm::vec3(0, 0, 1));
            model = glm::scale(model, glm::vec3(x.getScaling(), y.getScaling(), z.getScaling()));
            model = glm::translate(model, glm::vec3(x, y, z));
            return model;
        }
    public:
        Axis    x;
        Axis    y;
        Axis    z;
    };

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

        void build(size_t, VkRenderPass, VkExtent2D);
        void rebuild(size_t, VkRenderPass, VkExtent2D);
        void release(size_t);

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

