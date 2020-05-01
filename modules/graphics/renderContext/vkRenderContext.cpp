/**
 * Netero sources under BSD-3-Clause
 * see LICENSE.txt
 */

#include <stdexcept>


#include <netero/graphics/window.hpp>
#include <netero/graphics/physicalDevice.hpp>
#include "window/GLFWWindow.hpp"
#include "renderContext/vkRenderContext.hpp"

std::unique_ptr<netero::graphics::RenderContext> netero::graphics::RenderContext::_instance = nullptr;

netero::graphics::RenderContext::RenderContext(const std::string& appName,
        bool enableValidationLayers)
    :   pImpl(std::make_unique<RenderContext::impl>(appName, enableValidationLayers))
{}

netero::graphics::RenderContext::~RenderContext() = default;

void    netero::graphics::RenderContext::Initialize(const std::string& appName, bool enableValidationLayers) {
    RenderContext::_instance = std::unique_ptr<RenderContext>(new RenderContext(appName, enableValidationLayers));
    _instance->initialize();
}

netero::graphics::RenderContext*    netero::graphics::RenderContext::Instance() {
    return RenderContext::_instance.get();
}

void    netero::graphics::RenderContext::Release() {
    RenderContext::_instance->release();
    RenderContext::_instance.reset(nullptr);
}

netero::graphics::RenderContext::impl::impl(const std::string& _appName,
        bool _enableValidationLayers)
    :   appName(_appName),
        enableValidationLayers(_enableValidationLayers),
        instance(nullptr)
{
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName = this->appName.c_str();
}

netero::graphics::RenderContext::impl::~impl() = default;

void netero::graphics::RenderContext::initialize() const {
    this->pImpl->createVulkanContext();
    netero::graphics::PhysicalDevice::Initialize(this->pImpl->instance);
    if (!this->pImpl->logicalDevice) {
        this->setPhysicalDevice(PhysicalDevice::Instance()->getPhysicalDevice());
    }
}

void netero::graphics::RenderContext::release() const {
    if (this->pImpl->surface) {
        vkDestroySurfaceKHR(this->pImpl->instance, this->pImpl->surface, nullptr);
    }
    this->pImpl->releaseVulkanContext();
}

void netero::graphics::RenderContext::setTargetWindow(window* _window) const {
    const VkResult result = glfwCreateWindowSurface(this->pImpl->instance, _window->_pImpl->window, nullptr, &this->pImpl->surface);
    if (result != VK_SUCCESS) {
        throw std::runtime_error("Failed to create window surface.");
    }
}

