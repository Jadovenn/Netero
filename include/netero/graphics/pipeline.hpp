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

    struct Pipeline {
   private:
        void createSwapchain();
        //void createUniformBuffers(); // Model
        //void createDescriptorPool(); // Model
        //void createDescriptorSets(); // Model
        //void createDescriptorSetLayout(); // Model
        void createImageViews();
        void createRenderPass();
        //void createGraphicsPipeline(std::vector<Model*>&);
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
        //UniformBufferObject _ubo {};
        //VkDescriptorPool    _descriptorPool; // Model
        //VkDescriptorSetLayout           _descriptorSetLayout; // Model
        //std::vector<VkDescriptorSet>    _descriptorSets; // Model
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

        void buildModels(std::vector<Model*>&);
        void rebuildModels(std::vector<Model*>&);
        void releaseModels(std::vector<Model*>&);

        VkSwapchainKHR                  swapchain;
        VkExtent2D                      swapchainExtent = {0, 0};
        std::vector<VkImage>            swapchainImages;
        std::vector<VkCommandBuffer>    commandBuffers;
        //std::vector<VkBuffer>           uniformBuffers; // Model
        //std::vector<VkDeviceMemory>     uniformBuffersMemory; // Model
    };

}

