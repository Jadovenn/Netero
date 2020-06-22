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
         * Allocate and set up necessary ressources related to the context
         * and underlaying objects.
         * @details You should call this method once before performing
         * any update of the context.
         */
        void build();

        /**
         * Release any allocated ressources, the context become unusable
         * after any call of this method.
         */
        void release();


        void update();
        [[nodiscard]] bool shouldClose() const;

        [[nodiscard]] Model*    createModel();
        void                    deleteModel(Model*);

        [[nodiscard]] std::vector<std::string>  getPhysicalDevices() const;
        [[nodiscard]] std::string   getCurrentPhysicalDeviceName() const;

    private:
        void    createSemaphores();
        void    recreateSwapchain();
        bool    prepareFrame(uint32_t& frameIndex);
        void    submitFrame(uint32_t frameIndex);

        VkInstance  _vulkanInstance; /**< Vulkan instance*/
        int         _height; /**< Height of the surface. Might be updated if the swapchain is recreated. */
        int         _width; /**< Widht of the surface. Mught be updated if the swapchain is recreated. */
        WindowMode  _windowMode;
        const std::string   _name;
        const std::string   _deviceName;

        VkSurfaceKHR    _surface = nullptr;
        Device*         _device = nullptr;
        VertexBuffer*   _vertexBuffer = nullptr;
        Pipeline*       _pipeline = nullptr;
        std::vector<Shader>         _shaderModules;
        std::vector<Model*>         _models;
        std::vector<VkSemaphore>    _imageAvailableSemaphore;
        std::vector<VkSemaphore>    _renderFinishedSemaphore; /** */
        std::vector<VkFence>        _inFlightFences; /**< Fence to synchromise fences in flight. Synchronise GPU/CPU. */
        std::vector<VkFence>        _imagesInFlight; /**< Fence to synchronise frame in flight vs it is still on processing from GPU. Synchronise GPU/CPU. */
        size_t                      _currentFrame = 0; /**< The current index of the frame in processing. */
        const int   MAX_FRAMES_IN_FLIGHT = 2; /**< The maximum number of frame to be processed at the same time. */


        struct impl;
        std::unique_ptr<impl>   _pImpl; /**< Underlaying member that hide GLFW usage. */
    };
}

