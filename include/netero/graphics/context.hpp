/**
 * Netero sources under BSD-3-Clause
 * see LICENSE.txt
 */

#pragma once

#include <string>
#include <memory>
#include <vector>
#include <vulkan/vulkan.h>
#include <netero/graphics/window.hpp>
#include <netero/graphics/shader.hpp>

namespace netero::graphics {

    class Application;

    class Context {
        Context(VkInstance vulkanInstance,
            unsigned width,
            unsigned height,
            WindowMode mode,
            const std::string& name,
            const std::string& deviceName);
    public:
        friend Application;
        Context(Context&&) = delete;
        Context(const Context&) = delete;
        Context& operator=(const Context&) = delete;
        Context& operator=(Context&&) = delete;
        ~Context();

        // Context related
        void    run();
        int     loadShader(const std::string&, ShaderStage);

        // Device Related
        [[nodiscard]] std::vector<std::string>  getPhysicalDevices() const;
        [[nodiscard]] std::string   getCurrentPhysicalDeviceName() const;

    private:
        void    pickPhysicalDevice();
        void    createLogicalDevice(VkPhysicalDevice device);
        void    createSwapchain();
        void    createImageViews();
        void    createRenderPass();
        void    createGraphicsPipeline();
        void    createFrameBuffers();
        void    createCommandPool();
        void    createCommandBuffers();
        void    createSemaphores();
        void    drawFrame();
        void    cleanUpSwapchain();
        void    recreateSwapchain();

        VkInstance  _vulkanInstance;
        int    _height;
        int    _width;
        WindowMode  _windowMode;
        const std::string   _name;
        const std::string   _deviceName;

        VkPhysicalDevice    _physicalDevice = nullptr;
        VkDevice            _logicalDevice = nullptr;
        VkQueue             _graphicsQueue = nullptr;
        VkQueue             _presentQueue = nullptr;
        VkSurfaceKHR        _surface = nullptr;
        VkSwapchainKHR      _swapchain = nullptr;
        VkFormat            _swapchainImageFormat = VK_FORMAT_UNDEFINED;
        VkExtent2D          _swapchainExtent = {0, 0};
        VkRenderPass        _renderPass = nullptr;
        VkPipelineLayout    _pipelineLayout = nullptr;
        VkPipeline          _graphicsPipeline = nullptr;
        VkCommandPool       _commandPool = nullptr;
        std::vector<VkImage>        _swapchainImage;
        std::vector<VkImageView>    _swapchainImageViews;
        std::vector<VkFramebuffer>  _swapchainFrameBuffers;
        std::vector<Shader>         _shaderModules;
        std::vector<VkCommandBuffer>    _commandBuffers;

        std::vector<VkSemaphore>    _imageAvailableSemaphore;
        std::vector<VkSemaphore>    _renderFinishedSemaphore;
        std::vector<VkFence>        _inFlightFences;
        std::vector<VkFence>        _imagesInFlight;
        size_t      _currentFrame = 0;
        const int   MAX_FRAMES_IN_FLIGHT = 2;

        struct impl;
        std::unique_ptr<impl>   _pImpl;
    };
}

