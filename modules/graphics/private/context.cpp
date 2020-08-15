/**
 * Netero sources under BSD-3-Clause
 * see LICENSE.txt
 */

#define GLFW_INCLUDE_VULKAN
#define GLM_FORCE_DEPTH_ZERO_TO_ONE

#include <stdexcept>

#include <GLFW/glfw3.h>

#include <netero/graphics/context.hpp>
#include <netero/graphics/device.hpp>
#include <netero/graphics/pipeline.hpp>

#include "utils/vkUtils.hpp"

namespace netero::graphics {

struct Context::impl {
    GLFWwindow* window = nullptr;
};

Context::Context(VkInstance         vulkanInstance,
                 unsigned           width,
                 unsigned           height,
                 WindowMode         mode,
                 const std::string& name,       // NOLINT(modernize-pass-by-value)
                 const std::string& deviceName) // NOLINT(modernize-pass-by-value)
    : _vulkanInstance(vulkanInstance),
      _height(static_cast<int>(height)),
      _width(static_cast<int>(width)),
      _windowMode(mode),
      _name(name),
      _deviceName(deviceName),
      _pImpl(std::make_unique<Context::impl>())
{
    GLFWmonitor* monitor = nullptr;
    if (_windowMode == WindowMode::FIX) {
        glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
    }
    else if (_windowMode == WindowMode::BORDERLESS) {
        glfwWindowHint(GLFW_DECORATED, GLFW_FALSE);
    }
    else if (_windowMode == WindowMode::FULLSCREEN) {
        monitor = glfwGetPrimaryMonitor();
        glfwGetMonitorWorkarea(monitor, nullptr, nullptr, &this->_width, &this->_height);
    }
    this->_pImpl->window = glfwCreateWindow(static_cast<int>(this->_width),
                                            static_cast<int>(this->_height),
                                            this->_name.c_str(),
                                            monitor,
                                            nullptr);
    if (_windowMode == WindowMode::BORDERLESS) {
        monitor = glfwGetPrimaryMonitor();
        int wHeight, wWidth = 0;
        glfwGetMonitorWorkarea(monitor, nullptr, nullptr, &wWidth, &wHeight);
        wHeight /= 2;
        wHeight -= this->_height / 2;
        wWidth /= 2;
        wWidth -= this->_width / 2;
        glfwSetWindowPos(this->_pImpl->window, wWidth, wHeight);
    }
    const VkResult result = glfwCreateWindowSurface(this->_vulkanInstance,
                                                    this->_pImpl->window,
                                                    nullptr,
                                                    &this->_surface);
    if (result != VK_SUCCESS) {
        throw std::runtime_error("Failed to create window surface.");
    }
    this->_device = new netero::graphics::Device(this->_vulkanInstance, this->_surface);
    if (!this->_device) {
        throw std::bad_alloc();
    }
    this->_device->setSurfaceHeight(this->_height);
    this->_device->setSurfaceWidth(this->_width);
}

Context::~Context()
{
    delete this->_pipeline;
    for (auto* model : this->_models) {
        delete model;
    }
    for (unsigned idx = 0; static_cast<int>(idx) < this->MAX_FRAMES_IN_FLIGHT; idx++) {
        vkDestroySemaphore(this->_device->logicalDevice,
                           this->_imageAvailableSemaphore[idx],
                           nullptr);
        vkDestroySemaphore(this->_device->logicalDevice,
                           this->_renderFinishedSemaphore[idx],
                           nullptr);
        vkDestroyFence(this->_device->logicalDevice, this->_inFlightFences[idx], nullptr);
    }
    vkDestroySurfaceKHR(this->_vulkanInstance, this->_surface, nullptr);
    delete this->_device;
    glfwDestroyWindow(this->_pImpl->window);
}

Model* Context::createModel()
{
    auto* model = new Model(_vulkanInstance, _device);
    this->_models.push_back(model);
    return model;
}

void Context::deleteModel(Model* model)
{
    auto it = std::find(this->_models.begin(), this->_models.end(), model);
    if (it != this->_models.end()) {
        this->_models.erase(it);
        delete model;
    }
}

std::vector<std::string> Context::getPhysicalDevices() const
{
    return vkUtils::getDevicesName(this->_vulkanInstance);
}

std::string Context::getCurrentPhysicalDeviceName() const
{
    return this->_device->deviceName;
}

void Context::recreateSwapchain()
{
    vkDeviceWaitIdle(this->_device->logicalDevice);
    glfwGetFramebufferSize(this->_pImpl->window, &this->_width, &this->_height);
    while (this->_width == 0 || this->_height == 0) {
        glfwGetFramebufferSize(this->_pImpl->window, &this->_width, &this->_height);
        glfwWaitEvents();
    }
    this->_pipeline->rebuild(this->_models);
}

bool Context::shouldClose() const
{
    return glfwWindowShouldClose(this->_pImpl->window);
}

void Context::build()
{
    this->_pipeline = new Pipeline(this->_vulkanInstance, this->_device);
    if (!this->_pipeline) {
        throw std::bad_alloc();
    }
    this->_pipeline->build(this->_models);
    this->createSemaphores();
}

void Context::release()
{
    vkDeviceWaitIdle(this->_device->logicalDevice);
    this->_pipeline->releaseModels(this->_models);
    delete this->_pipeline;
    this->_pipeline = nullptr;
}

bool Context::prepareFrame(uint32_t& frameIndex)
{
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
                                            &frameIndex);
    if (result == VK_ERROR_OUT_OF_DATE_KHR) {
        this->recreateSwapchain();
        return false;
    }
    if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
        throw std::runtime_error("Failed to retrieve next image");
    }

    if (this->_imagesInFlight[frameIndex] != nullptr) {
        vkWaitForFences(this->_device->logicalDevice,
                        1,
                        &this->_imagesInFlight[frameIndex],
                        VK_TRUE,
                        UINT64_MAX);
    }
    this->_imagesInFlight[frameIndex] = this->_inFlightFences[this->_currentFrame];
    return true;
}

void Context::submitFrame(uint32_t frameIndex)
{
    VkSemaphore          waitSemaphores[] = { this->_imageAvailableSemaphore[this->_currentFrame] };
    VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
    VkSubmitInfo         submitInfo {};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.waitSemaphoreCount = 1;
    submitInfo.pWaitSemaphores = waitSemaphores;
    submitInfo.pWaitDstStageMask = waitStages;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &this->_pipeline->commandBuffers[frameIndex];
    VkSemaphore signalSemaphores[] = { this->_renderFinishedSemaphore[this->_currentFrame] };
    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores = signalSemaphores;
    vkResetFences(this->_device->logicalDevice, 1, &this->_inFlightFences[this->_currentFrame]);
    VkResult result = vkQueueSubmit(this->_device->graphicsQueue,
                                    1,
                                    &submitInfo,
                                    this->_inFlightFences[this->_currentFrame]);
    if (result != VK_SUCCESS) {
        throw std::runtime_error("Failed to submit draw command buffer.");
    }
    VkPresentInfoKHR presentInfo {};
    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores = signalSemaphores;
    VkSwapchainKHR swapChains[] = { this->_pipeline->swapchain };
    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = swapChains;
    presentInfo.pImageIndices = &frameIndex;
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

void Context::createSemaphores()
{
    this->_imageAvailableSemaphore.resize(this->MAX_FRAMES_IN_FLIGHT);
    this->_renderFinishedSemaphore.resize(this->MAX_FRAMES_IN_FLIGHT);
    this->_inFlightFences.resize(this->MAX_FRAMES_IN_FLIGHT);
    this->_imagesInFlight.resize(this->_pipeline->swapchainImages.size(), nullptr);

    VkSemaphoreCreateInfo semaphoreInfo {};
    semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
    VkFenceCreateInfo fenceInfo {};
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
        result = vkCreateFence(this->_device->logicalDevice,
                               &fenceInfo,
                               nullptr,
                               &this->_inFlightFences[idx]);
        if (result != VK_SUCCESS) {
            throw std::runtime_error("Failed to create inFlight fence.");
        }
    }
}

void Context::update()
{
    uint32_t frameIndex = 0;
    glfwPollEvents();
    if (!this->prepareFrame(frameIndex)) {
        return;
    }
    this->_pipeline->update(frameIndex);
    for (auto* model : this->_models) {
        model->update(frameIndex);
    }
    this->submitFrame(frameIndex);
}
} // namespace netero::graphics
