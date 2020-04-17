/**
 * Netero sources under BSD-3-Clause
 * see LICENSE.txt
 */

#include "VulkanRenderContext.hpp"

netero::graphics::RenderContext::RenderContext(const std::string& appName)
    :   pImpl(std::make_unique<RenderContext::impl>(appName))
{}

netero::graphics::RenderContext::~RenderContext() = default;


netero::graphics::RenderContext::impl::impl(const std::string& appName)
	:   appName(appName)
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
}

void netero::graphics::RenderContext::release() const {
    this->pImpl->releaseVulkanContext();
}



