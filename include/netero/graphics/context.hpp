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
#include <netero/graphics/model.hpp>

namespace netero::graphics {

    class Application;

    // see: https://www.khronos.org/registry/vulkan/specs/1.1-extensions/html/chap14.html#interfaces-resources-layout
    // for alignment requirements
    struct UniformBufferObject {
        glm::mat4   view;
        glm::mat4   proj;
    };

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

        [[nodiscard]] Model*  createModel();
        void    deleteModel(Model*);

        // Device Related
        [[nodiscard]] std::vector<std::string>  getPhysicalDevices() const;
        [[nodiscard]] std::string   getCurrentPhysicalDeviceName() const;

    private:
        //void createUniformBuffers(size_t);
        //void createDescriptorPool(size_t);
        //void createDescriptorSets(size_t);
        //void createDescriptorSetLayout();
        void createSemaphores();
        void drawFrame();
        void recreateSwapchain();
        void    update(uint32_t imageIndex);

        VkInstance  _vulkanInstance;
        int         _height;
        int         _width;
        WindowMode  _windowMode;
        const std::string   _name;
        const std::string   _deviceName;

        VkSurfaceKHR    _surface = nullptr;
        Device*         _device = nullptr;
        VertexBuffer*   _vertexBuffer = nullptr;
        Pipeline*       _pipeline = nullptr;
        std::vector<Shader>         _shaderModules;
        std::vector<Model*>          _models;
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

