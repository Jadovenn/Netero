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
#include <netero/graphics/device.hpp>
#include <netero/graphics/shader.hpp>
#include <netero/graphics/vertex.hpp>
#include <netero/graphics/pipeline.hpp>

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
        void    addVertices(std::vector<Vertex>&);
        void    addVertices(std::vector<Vertex>&, std::vector<uint16_t>&);

        // Device Related
        [[nodiscard]] std::vector<std::string>  getPhysicalDevices() const;
        [[nodiscard]] std::string   getCurrentPhysicalDeviceName() const;

    private:
        void    createSemaphores();
        void    drawFrame();
        void    recreateSwapchain();

        VkInstance  _vulkanInstance;
        int    _height;
        int    _width;
        WindowMode  _windowMode;
        const std::string   _name;
        const std::string   _deviceName;

        VkSurfaceKHR    _surface = nullptr;
        Device*         _device = nullptr;
        VertexBuffer*   _vertexBuffer = nullptr;
        Pipeline*       _pipeline = nullptr;
        std::vector<Shader>         _shaderModules;
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

