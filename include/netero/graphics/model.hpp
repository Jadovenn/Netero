/**
 * Netero sources under BSD-3-Clause
 * see LICENSE.txt
 */

#pragma once

#include <vector>
#include <atomic>
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
        volatile float      _value = 0;
        volatile float      _rotation = 0;
        volatile float      _scale = 1;
        std::atomic<bool>   _has_changed;
    public:
        operator bool() const { return _has_changed; }
        operator float() const { return this->_value; }
        float operator=(float value) { this->_value = value; return this->_value; };

        void rotate(float value) { this->_rotation = value; }
        float getRotation() const { return this->_rotation; }
        void scale(float value) { this->_scale = value; }
        float getScaling() const { return this->_scale; }
    };

    /**
     * @brief Actual instance of a model
     * An instance represent one model in the context
     * that is actually in the graphics pipeline.
     * Many instance of a model may exist.
     */
    class Instance {
        friend class Model;

        struct InstanceData {
            glm::mat4   model;
        };

        InstanceData    data;
        int             offset = 0;

        static  VkVertexInputBindingDescription  getBindingDescription() {
            VkVertexInputBindingDescription bindingDescription = {};

            bindingDescription.binding = 1;
            bindingDescription.stride = sizeof(InstanceData);
            bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_INSTANCE;
            return bindingDescription;
        }

        static std::array<VkVertexInputAttributeDescription, 4>    getAttributeDescription() {
            std::array<VkVertexInputAttributeDescription, 4>    attributeDescriptions{};
            attributeDescriptions[0].binding = 1;
            attributeDescriptions[0].location = 2;
            attributeDescriptions[0].format = VK_FORMAT_R32G32B32A32_SFLOAT;
            attributeDescriptions[0].offset = 0;
            attributeDescriptions[1].binding = 1;
            attributeDescriptions[1].location = 3;
            attributeDescriptions[1].format = VK_FORMAT_R32G32B32A32_SFLOAT;
            attributeDescriptions[1].offset = sizeof(glm::vec4);
            attributeDescriptions[2].binding = 1;
            attributeDescriptions[2].location = 4;
            attributeDescriptions[2].format = VK_FORMAT_R32G32B32A32_SFLOAT;
            attributeDescriptions[2].offset = sizeof(glm::vec4) * 2;
            attributeDescriptions[3].binding = 1;
            attributeDescriptions[3].location = 5;
            attributeDescriptions[3].format = VK_FORMAT_R32G32B32A32_SFLOAT;
            attributeDescriptions[3].offset = sizeof(glm::vec4) * 3;

            return attributeDescriptions;
        }

        // Need to recheck the order of the operation
        glm::mat4* getModelMat() {
            this->data.model = glm::mat4(1.f);
            this->data.model = glm::translate(this->data.model, glm::vec3(static_cast<float>(x), static_cast<float>(y), static_cast<float>(z)));
            this->data.model = glm::rotate(this->data.model, x.getRotation(), glm::vec3(1, 0, 0));
            this->data.model = glm::rotate(this->data.model, y.getRotation(), glm::vec3(0, 1, 0));
            this->data.model = glm::rotate(this->data.model, z.getRotation(), glm::vec3(0, 0, 1));
            this->data.model = glm::scale(this->data.model, glm::vec3(x.getScaling(), y.getScaling(), z.getScaling()));
            return &this->data.model;
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

        void createInstanceBuffer();
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

        // Model Buffer
        VkBuffer            instanceBuffer;
        VkDeviceMemory      instanceBufferMemory;
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

