/**
 * Netero sources under BSD-3-Clause
 * see LICENSE.txt
 */

#include <netero/graphics/physicalDevice.hpp>
#include "renderContext/vkRenderContext.hpp"

netero::graphics::RenderContext::RenderContext(const std::string& appName,
        bool enableValidationLayers)
    :   pImpl(std::make_unique<RenderContext::impl>(appName, enableValidationLayers))
{}

netero::graphics::RenderContext::~RenderContext() = default;


netero::graphics::RenderContext::impl::impl(const std::string& _appName,
        bool _enableValidationLayers)
    :   appName(_appName),
        enableValidationLayers(_enableValidationLayers),
        instance(nullptr)
{
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName = this->appName.c_str();
    appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.pEngineName = "Netero";
    appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.apiVersion = VK_API_VERSION_1_0;
}

netero::graphics::RenderContext::impl::~impl() = default;

void netero::graphics::RenderContext::initialize() const {
    this->pImpl->createVulkanContext();
    netero::graphics::PhysicalDevice::Initialize(this->pImpl->instance);
}

void netero::graphics::RenderContext::release() const {
    this->pImpl->releaseVulkanContext();
}

