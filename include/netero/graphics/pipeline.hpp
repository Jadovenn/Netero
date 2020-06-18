/**
 * Netero sources under BSD-3-Clause
 * see LICENSE.txt
 */

#pragma once

#include <vector>
#include <vulkan/vulkan.h>
#include <netero/graphics/device.hpp>
#include <netero/graphics/descriptor.hpp>

namespace netero::graphics {

    class Model;

    struct Pipeline {
   private:
        void createSwapchain();
        void createUniformBuffers();
        //void createDescriptorPool();
        //void createDescriptorSets();
        //void createDescriptorSetLayoutBinding();
        void createImageViews();
        void createRenderPass();
        void createFrameBuffers();
        void createCommandPool();
        void createCommandBuffers(std::vector<Model*>&);

        VkInstance          _instance;
        uint32_t            _imageCount;
        Device*             _device;
        VkRenderPass        _renderPass;
        VkPipelineLayout    _pipelineLayout;
        VkPipeline          _graphicsPipeline;
        VkCommandPool       _commandPool;
        VkFormat            _swapchainImageFormat = VK_FORMAT_UNDEFINED;
        std::vector<VkImageView>        _swapchainImageViews;
        std::vector<VkFramebuffer>      _swapchainFrameBuffers;

        // UBO related
        std::vector<VkBuffer>           _uniformBuffers;
        std::vector<VkDeviceMemory>     _uniformBuffersMemory;
        DescriptorSets                  _uniformBufferDescriptorSets;
        //VkDescriptorPool                _descriptorPool;
        //VkDescriptorSetLayout           _descriptorSetLayout;
        //std::vector<VkDescriptorSet>    _descriptorSets;

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
        void    update(uint32_t);

        void buildModels(std::vector<Model*>&) const;
        void rebuildModels(std::vector<Model*>&) const;
        void releaseModels(std::vector<Model*>&) const;

        VkSwapchainKHR                  swapchain;
        VkExtent2D                      swapchainExtent = {0, 0};
        std::vector<VkImage>            swapchainImages;
        std::vector<VkCommandBuffer>    commandBuffers;
    };

}

