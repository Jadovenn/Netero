/**
 * Netero sources under BSD-3-Clause
 * see LICENSE.txt
 */

#pragma once

#include <vector>
#include <vulkan/vulkan.h>
#include <glm/glm.hpp>
#include <netero/graphics/device.hpp>

namespace netero::graphics {

    class Model;

    // see: https://www.khronos.org/registry/vulkan/specs/1.1-extensions/html/chap14.html#interfaces-resources-layout
    // for alignment requirements
    struct UniformBufferObject {
        glm::mat4   model;
        glm::mat4   view;
        glm::mat4   proj;
    };

    struct Pipeline {
   private:
        void createSwapchain();
        void createUniformBuffers();
        void createDescriptorPool();
        void createDescriptorSets();
        void createDescriptorSetLayout();
        void createImageViews();
        void createRenderPass(std::vector<Model*>&);
        void createGraphicsPipeline(std::vector<Model*>&);
        void createFrameBuffers();
        void createCommandPool();
        void createCommandBuffers(std::vector<Model*>&);

        VkInstance          _instance;
        Device*             _device;
        VkRenderPass        _renderPass;
        VkPipelineLayout    _pipelineLayout;
        VkPipeline          _graphicsPipeline;
        VkCommandPool       _commandPool;
        VkFormat            _swapchainImageFormat = VK_FORMAT_UNDEFINED;
        UniformBufferObject _ubo {};
        VkDescriptorPool    _descriptorPool;
        VkDescriptorSetLayout           _descriptorSetLayout;
        std::vector<VkDescriptorSet>    _descriptorSets;
        std::vector<VkImageView>        _swapchainImageViews;
        std::vector<VkFramebuffer>      _swapchainFrameBuffers;
    public:
        Pipeline(VkInstance, Device*);
        Pipeline(const Pipeline&) = delete;
        Pipeline(Pipeline&&) = delete;
        Pipeline& operator=(const Pipeline&) = delete;
        Pipeline& operator=(Pipeline&&) = delete;
        ~Pipeline();

        void    build(std::vector<Model*>&);
        void    rebuild(std::vector<Model*>&);
        void    release();

        VkSwapchainKHR                  swapchain;
        VkExtent2D                      swapchainExtent = {0, 0};
        std::vector<VkImage>            swapchainImages;
        std::vector<VkCommandBuffer>    commandBuffers;
        std::vector<VkBuffer>           uniformBuffers;
        std::vector<VkDeviceMemory>     uniformBuffersMemory;
    };

}

