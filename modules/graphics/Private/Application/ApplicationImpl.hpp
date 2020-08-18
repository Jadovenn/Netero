/**
 * Netero sources under BSD-3-Clause
 * see LICENSE.txt
 */

#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <Netero/Graphics/Application.hpp>

namespace Netero::Gfx {

class Application::Impl {
    public:
    explicit Impl(const std::string& anApplicationName);
    ~Impl();

    const std::string myApplicationName;
    VkApplicationInfo myVulkanApplicationInfo;
    VkInstance        myVulkanInstance;

    VkDebugUtilsMessengerEXT myDebugMessenger = nullptr;
    VkDebugReportCallbackEXT myDebugReport = nullptr;
};

} // namespace Netero::Gfx
