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
        this->pickPhysicalDevice();
        this->createLogicalDevice(this->_physicalDevice);
        this->createSwapchain();
        this->createImageViews();
    }

    Context::~Context() {
        cleanUpSwapchain();
        for (const auto& shader: this->_shaderModules) {
            vkDestroyShaderModule(this->_logicalDevice, shader.shaderModule, nullptr);
        }
        for (unsigned idx = 0; static_cast<int>(idx) < this->MAX_FRAMES_IN_FLIGHT; idx++) {
            vkDestroySemaphore(this->_logicalDevice, this->_imageAvailableSemaphore[idx], nullptr);
            vkDestroySemaphore(this->_logicalDevice, this->_renderFinishedSemaphore[idx], nullptr);
            vkDestroyFence(this->_logicalDevice, this->_inFlightFences[idx], nullptr);
        }
        vkDestroyCommandPool(this->_logicalDevice, this->_commandPool, nullptr);
        vkDestroySurfaceKHR(this->_vulkanInstance, this->_surface, nullptr);
        vkDestroyDevice(this->_logicalDevice, nullptr);
        glfwDestroyWindow(this->_pImpl->window);
    }

    void Context::cleanUpSwapchain() {
        for (auto* frameBuffer : this->_swapchainFrameBuffers) {
            vkDestroyFramebuffer(this->_logicalDevice, frameBuffer, nullptr);
        }
        vkFreeCommandBuffers(this->_logicalDevice, this->_commandPool, static_cast<uint32_t>(this->_commandBuffers.size()), this->_commandBuffers.data());
        vkDestroyPipeline(this->_logicalDevice, this->_graphicsPipeline, nullptr);
        vkDestroyPipelineLayout(this->_logicalDevice, this->_pipelineLayout, nullptr);
        vkDestroyRenderPass(this->_logicalDevice, this->_renderPass, nullptr);
        for (auto* imageView: this->_swapchainImageViews) {
            vkDestroyImageView(this->_logicalDevice, imageView, nullptr);
        }
        vkDestroySwapchainKHR(this->_logicalDevice, this->_swapchain, nullptr);
    }

    void Context::recreateSwapchain() {
        vkDeviceWaitIdle(this->_logicalDevice);
        glfwGetFramebufferSize(this->_pImpl->window, &this->_width, &this->_height);
        while (this->_width == 0 || this->_height == 0) {
            glfwGetFramebufferSize(this->_pImpl->window, &this->_width, &this->_height);
            glfwWaitEvents();
        }
        this->cleanUpSwapchain();
        this->createSwapchain();
        this->createImageViews();
        this->createRenderPass();
        this->createGraphicsPipeline();
        this->createFrameBuffers();
        this->createCommandBuffers();
    }

    void Context::run() {
        this->createRenderPass();
        this->createGraphicsPipeline();
        this->createFrameBuffers();
        this->createCommandPool();
        this->createCommandBuffers();
        this->createSemaphores();
        while (!glfwWindowShouldClose(this->_pImpl->window)) {
            glfwPollEvents();
            this->drawFrame();
        }
        vkDeviceWaitIdle(this->_logicalDevice);
    }

    void Context::drawFrame() {
        uint32_t imageIndex;
        vkWaitForFences(this->_logicalDevice,
                1,
                &this->_inFlightFences[this->_currentFrame],
                VK_TRUE,
                UINT64_MAX);
        VkResult result = vkAcquireNextImageKHR(this->_logicalDevice,
            this->_swapchain,
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
            vkWaitForFences(this->_logicalDevice,
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
        submitInfo.pCommandBuffers = &this->_commandBuffers[imageIndex];
        VkSemaphore signalSemaphores[] = { this->_renderFinishedSemaphore[this->_currentFrame] };
        submitInfo.signalSemaphoreCount = 1;
        submitInfo.pSignalSemaphores = signalSemaphores;
        vkResetFences(this->_logicalDevice,
                      1,
                      &this->_inFlightFences[this->_currentFrame]);
        result = vkQueueSubmit(this->_graphicsQueue,
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
        VkSwapchainKHR swapChains[] = { this->_swapchain };
        presentInfo.swapchainCount = 1;
        presentInfo.pSwapchains = swapChains;
        presentInfo.pImageIndices = &imageIndex;
        presentInfo.pResults = nullptr;
        result = vkQueuePresentKHR(this->_presentQueue, &presentInfo);
        if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR) {
            this->recreateSwapchain();
        }
        else if (result != VK_SUCCESS) {
            throw std::runtime_error("Failed to present swapchain image.");
        }
        this->_currentFrame = (this->_currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
    }

}

