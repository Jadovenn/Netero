/**
 * Netero sources under BSD-3-Clause
 * see LICENSE.txt
 */

// ReSharper disable CppUnreachableCode

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <stdexcept>
#include <netero/graphics/context.hpp>
#include <utils/vkUtils.hpp>

namespace netero::graphics {

    struct Context::impl {
        GLFWwindow* window = nullptr;
    };

    Context::Context(VkInstance vulkanInstance,
        unsigned width,
        unsigned height,
        WindowMode mode,
        const std::string& name, // NOLINT(modernize-pass-by-value)
        const std::string& deviceName)  // NOLINT(modernize-pass-by-value)
        :   _vulkanInstance(vulkanInstance),
            _height(static_cast<int>(height)),
            _width(static_cast<int>(width)),
            _windowMode(mode),
            _name(name),
            _deviceName(deviceName),
            _pImpl(std::make_unique<Context::impl>())
    {
        // Init GLFW Window
        if (_windowMode == WindowMode::FIX) {
            glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
        }
        this->_pImpl->window = glfwCreateWindow(static_cast<int>(this->_width),
            static_cast<int>(this->_height),
            this->_name.c_str(),
            nullptr,
            nullptr);
        const VkResult result = glfwCreateWindowSurface(this->_vulkanInstance,
            this->_pImpl->window,
            nullptr,
            &this->_surface);
        if (result != VK_SUCCESS) {
            throw std::runtime_error("Failed to create window surface.");
        }
        this->_device = new netero::graphics::Device(this->_vulkanInstance, this->_surface);
        if (!this->_device) { throw std::bad_alloc(); }
        this->_device->setSurfaceHeight(height);
        this->_device->setSurfaceWidth(width);
        this->_vertexBuffer = new VertexBuffer(this->_device);
        if (!this->_vertexBuffer) { throw std::bad_alloc(); }
    }

    Context::~Context() {
        delete this->_pipeline;
        delete this->_vertexBuffer;
        for (const auto& shader: this->_shaderModules) {
            vkDestroyShaderModule(this->_device->logicalDevice, shader.shaderModule, nullptr);
        }
        for (unsigned idx = 0; static_cast<int>(idx) < this->MAX_FRAMES_IN_FLIGHT; idx++) {
            vkDestroySemaphore(this->_device->logicalDevice, this->_imageAvailableSemaphore[idx], nullptr);
            vkDestroySemaphore(this->_device->logicalDevice, this->_renderFinishedSemaphore[idx], nullptr);
            vkDestroyFence(this->_device->logicalDevice, this->_inFlightFences[idx], nullptr);
        }
        vkDestroySurfaceKHR(this->_vulkanInstance, this->_surface, nullptr);
        delete this->_device;
        glfwDestroyWindow(this->_pImpl->window);
    }

    void Context::recreateSwapchain() {
        vkDeviceWaitIdle(this->_device->logicalDevice);
        glfwGetFramebufferSize(this->_pImpl->window, &this->_width, &this->_height);
        while (this->_width == 0 || this->_height == 0) {
            glfwGetFramebufferSize(this->_pImpl->window, &this->_width, &this->_height);
            glfwWaitEvents();
        }
        this->_pipeline->rebuild(_shaderModules, *this->_vertexBuffer);
    }

    void Context::run() {
        this->_pipeline = new Pipeline(this->_vulkanInstance, this->_device);
        if (!this->_pipeline) { throw std::bad_alloc(); }
        this->_vertexBuffer->transfer();
        this->_pipeline->build(this->_shaderModules, *this->_vertexBuffer);
        this->createSemaphores();
        while (!glfwWindowShouldClose(this->_pImpl->window)) {
            glfwPollEvents();
            this->drawFrame();
        }
        vkDeviceWaitIdle(this->_device->logicalDevice);
        delete this->_pipeline;
        this->_pipeline = nullptr;
    }

    void Context::drawFrame() {
        uint32_t imageIndex;
        vkWaitForFences(this->_device->logicalDevice,
                1,
                &this->_inFlightFences[this->_currentFrame],
                VK_TRUE,
                UINT64_MAX);
        VkResult result = vkAcquireNextImageKHR(this->_device->logicalDevice,
            this->_pipeline->swapchain,
            UINT64_MAX,
            this->_imageAvailableSemaphore[this->_currentFrame],
            nullptr,
            &imageIndex);
        if (result == VK_ERROR_OUT_OF_DATE_KHR) {
            this->recreateSwapchain();
            return;
        }
        else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
            throw std::runtime_error("Failed to retrieve next image");
        }

        if (this->_imagesInFlight[imageIndex] != nullptr) {
            vkWaitForFences(this->_device->logicalDevice,
                    1,
                    &this->_imagesInFlight[imageIndex],
                    VK_TRUE,
                    UINT64_MAX);
        }
        this->_imagesInFlight[imageIndex] = this->_inFlightFences[this->_currentFrame];

        VkSemaphore waitSemaphores[] = { this->_imageAvailableSemaphore[this->_currentFrame] };
        VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
        VkSubmitInfo submitInfo{};
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        submitInfo.waitSemaphoreCount = 1;
        submitInfo.pWaitSemaphores = waitSemaphores;
        submitInfo.pWaitDstStageMask = waitStages;
        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = &this->_pipeline->commandBuffers[imageIndex];
        VkSemaphore signalSemaphores[] = { this->_renderFinishedSemaphore[this->_currentFrame] };
        submitInfo.signalSemaphoreCount = 1;
        submitInfo.pSignalSemaphores = signalSemaphores;
        vkResetFences(this->_device->logicalDevice,
                      1,
                      &this->_inFlightFences[this->_currentFrame]);
        result = vkQueueSubmit(this->_device->graphicsQueue,
            1,
            &submitInfo,
            this->_inFlightFences[this->_currentFrame]);
        if (result != VK_SUCCESS) {
            throw std::runtime_error("Failed to submit draw command buffer.");
        }
        VkPresentInfoKHR presentInfo{};
        presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
        presentInfo.waitSemaphoreCount = 1;
        presentInfo.pWaitSemaphores = signalSemaphores;
        VkSwapchainKHR swapChains[] = { this->_pipeline->swapchain };
        presentInfo.swapchainCount = 1;
        presentInfo.pSwapchains = swapChains;
        presentInfo.pImageIndices = &imageIndex;
        presentInfo.pResults = nullptr;
        result = vkQueuePresentKHR(this->_device->presentQueue, &presentInfo);
        if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR) {
            this->recreateSwapchain();
        }
        else if (result != VK_SUCCESS) {
            throw std::runtime_error("Failed to present swapchain image.");
        }
        this->_currentFrame = (this->_currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
    }

    void Context::createSemaphores() {
        this->_imageAvailableSemaphore.resize(this->MAX_FRAMES_IN_FLIGHT);
        this->_renderFinishedSemaphore.resize(this->MAX_FRAMES_IN_FLIGHT);
        this->_inFlightFences.resize(this->MAX_FRAMES_IN_FLIGHT);
        this->_imagesInFlight.resize(this->_pipeline->swapchainImages.size(), nullptr);

        VkSemaphoreCreateInfo semaphoreInfo{};
        semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
        VkFenceCreateInfo fenceInfo{};
        fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
        fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;
        for (unsigned idx = 0; idx < this->MAX_FRAMES_IN_FLIGHT; idx++) {
            VkResult result = vkCreateSemaphore(this->_device->logicalDevice,
                &semaphoreInfo,
                nullptr,
                &this->_imageAvailableSemaphore[idx]);
            if (result != VK_SUCCESS) {
                throw std::runtime_error("Failed to create imageAvailable semaphore.");
            }
            result = vkCreateSemaphore(this->_device->logicalDevice,
                &semaphoreInfo,
                nullptr,
                &this->_renderFinishedSemaphore[idx]);
            if (result != VK_SUCCESS) {
                throw std::runtime_error("Failed to create renderFinished semaphore.");
            }
            result = vkCreateFence(this->_device->logicalDevice, &fenceInfo, nullptr, &this->_inFlightFences[idx]);
            if (result != VK_SUCCESS) {
                throw std::runtime_error("Failed to create inFlight fence.");
            }
        }
    }

}

