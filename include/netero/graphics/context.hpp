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
        void    createLogicalDevice(VkPhysicalDevice device);
        void    createSwapchain();
        void    createImageViews();
        void    createRenderPass();
        void    createGraphicsPipeline();

        VkInstance  _vulkanInstance;
        unsigned    _height;
        unsigned    _width;
        WindowMode  _windowMode;
        const std::string   _name;
        const std::string   _deviceName;

        VkPhysicalDevice    _physicalDevice = nullptr;
        VkDevice            _logicalDevice = nullptr;
        VkQueue             _graphicsQueue = nullptr;
        VkQueue             _presentQueue = nullptr;
        VkSurfaceKHR        _surface = nullptr;
        VkSwapchainKHR      _swapchain = nullptr;
        VkFormat            _swapchainImageFormat;
        VkExtent2D          _swapchainExtent;
        VkRenderPass        _renderPass;
        VkPipelineLayout    _pipelineLayout;
        VkPipeline          _graphicsPipeline;
        std::vector<VkImage>        _swapchainImage;
        std::vector<VkImageView>    _swapchainImageViews;
        std::vector<Shader>         _shaderModules;

        struct impl;
        std::unique_ptr<impl>   _pImpl;
    };
}

