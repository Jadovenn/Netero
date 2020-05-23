/**
 * Netero sources under BSD-3-Clause
 * see LICENSE.txt
 */

#pragma once

#include <vector>
#include <vulkan/vulkan.h>
#include <netero/graphics/device.hpp>
#include <netero/graphics/shader.hpp>
#include <netero/graphics/vertex.hpp>

namespace netero::graphics {

    struct Pipeline {
   private:
        void createSwapchain();
        void createImageViews();
        void createRenderPass();
        void createGraphicsPipeline(std::vector<Shader>&);
        void createFrameBuffers();
        void createCommandPool();
        void createCommandBuffers(VertexBuffer&);

        VkInstance          _instance;
        Device*             _device;
        VkRenderPass        _renderPass;
        VkPipelineLayout    _pipelineLayout;
        VkPipeline          _graphicsPipeline;
        VkCommandPool       _commandPool;
        VkFormat            _swapchainImageFormat = VK_FORMAT_UNDEFINED;
        VkExtent2D          _swapchainExtent = {0, 0};
        std::vector<VkImageView>        _swapchainImageViews;
        std::vector<VkFramebuffer>      _swapchainFrameBuffers;
    public:
        Pipeline(VkInstance, Device*);
        Pipeline(const Pipeline&) = delete;
        Pipeline(Pipeline&&) = delete;
        Pipeline& operator=(const Pipeline&) = delete;
        Pipeline& operator=(Pipeline&&) = delete;
        ~Pipeline();

        void    build(std::vector<Shader>&, VertexBuffer&);
        void    rebuild(std::vector<Shader>&, VertexBuffer&);
        void    release();

        VkSwapchainKHR                  swapchain;
        std::vector<VkImage>            swapchainImages;
        std::vector<VkCommandBuffer>    commandBuffers;
    };

}

