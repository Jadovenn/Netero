/**
 * Netero sources under BSD-3-Clause
 * see LICENSE.txt
 */

#pragma once

#pragma warning(push)
#pragma warning(disable : 26812)
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#pragma warning(pop)

#include <Netero/Graphics/Application.hpp>

#include <Window/WindowFactory.hpp>
#include <Vulkan/Context/Context.hpp>

namespace Netero::Gfx {

class VulkanApplication {
    public:
    explicit VulkanApplication(const std::string& anApplicationName);
    ~VulkanApplication();

    static VkInstance GetInstance();

    const std::string   myApplicationName;
    VkApplicationInfo   myVulkanApplicationInfo;
    VkInstance          myVulkanInstance;

    VkDebugUtilsMessengerEXT myDebugMessenger = nullptr;
    VkDebugReportCallbackEXT myDebugReport = nullptr;

    WindowFactory myWindowFactory;

    std::vector<std::shared_ptr<Window>> myWindows;
};

} // namespace Netero::Gfx
