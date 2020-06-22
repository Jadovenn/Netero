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
        friend Application;
        Context(VkInstance vulkanInstance,
            unsigned width,
            unsigned height,
            WindowMode mode,
            const std::string& name,
            const std::string& deviceName);
    public:
        Context(Context&&) = delete;
        Context(const Context&) = delete;
        Context& operator=(const Context&) = delete;
        Context& operator=(Context&&) = delete;
        ~Context();

        /**
         * Allocate and set up necessary resources related to the context
         * and underlying objects.
         * @details You should call this method once before performing
         * any update of the context.
         */
        void build();

        /**
         * Release any allocated resources, the context become unusable
         * after any call of this method.
         */
        void release();

        /**
         * Update the context, this will basically process a frame.
         */
        void update();

        /**
         * Return true if the context must be destroy.
         */
        [[nodiscard]] bool shouldClose() const;

        /**
         * Create a model to be rendered.
         */
        [[nodiscard]] Model*    createModel();

        /**
         * Destroy a model.
         */
        void                    deleteModel(Model*);

        /**
         * Return the list of physical GPU devices.
         */
        [[nodiscard]] std::vector<std::string>  getPhysicalDevices() const;

        /**
         * Return the name of the GPU device in use.
         */
        [[nodiscard]] std::string   getCurrentPhysicalDeviceName() const;

    private:
        void    createSemaphores();
        void    recreateSwapchain();
        bool    prepareFrame(uint32_t& frameIndex);
        void    submitFrame(uint32_t frameIndex);

        VkInstance  _vulkanInstance; /**< Vulkan instance*/
        int         _height; /**< Height of the surface. Might be updated if the swapchain is recreated. */
        int         _width; /**< Width of the surface. Might be updated if the swapchain is recreated. */
        WindowMode  _windowMode;
        const std::string   _name;
        const std::string   _deviceName;

        VkSurfaceKHR    _surface = nullptr;
        Device*         _device = nullptr;
        VertexBuffer*   _vertexBuffer = nullptr;
        Pipeline*       _pipeline = nullptr;
        std::vector<Shader>         _shaderModules;
        std::vector<Model*>         _models; /** Vector of model. */
        std::vector<VkSemaphore>    _imageAvailableSemaphore; /**< Semaphore synchronizing available image to be rendered. */
        std::vector<VkSemaphore>    _renderFinishedSemaphore; /**< Semaphore synchronizing end of frame. */
        std::vector<VkFence>        _inFlightFences; /**< Fence to synchronise fences in flight. Synchronise GPU/CPU. */
        std::vector<VkFence>        _imagesInFlight; /**< Fence to synchronise frame in flight vs it is still on processing from GPU. Synchronise GPU/CPU. */
        size_t                      _currentFrame = 0; /**< The current index of the frame in processing. */
        const int   MAX_FRAMES_IN_FLIGHT = 2; /**< The maximum number of frame to be processed at the same time. */


        struct impl;
        std::unique_ptr<impl>   _pImpl; /**< Underlying member that hide GLFW usage. */
    };
}

