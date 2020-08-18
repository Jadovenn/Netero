/**
 * Netero sources under BSD-3-Clause
 * see LICENSE.txt
 */

#include <memory>

#include <Vulkan/VkUtils.hpp>

#include <netero/netero.hpp>

#include <Application/ApplicationImpl.hpp>

namespace Netero::Gfx {

Application::Impl::Impl(const std::string& anApplicationName)
    : myApplicationName(anApplicationName), myVulkanApplicationInfo {}, myVulkanInstance(nullptr)
{
}

Application::Impl::~Impl() = default;

Application::RtCode Application::Initialize(const std::string& anApplicationName)
{
    Application::myImpl = std::make_unique<Application::Impl>(anApplicationName);
    glfwInit();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    myImpl->myVulkanApplicationInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    myImpl->myVulkanApplicationInfo.pApplicationName = myImpl->myApplicationName.c_str();
    myImpl->myVulkanApplicationInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    myImpl->myVulkanApplicationInfo.pEngineName = "Netero";
    myImpl->myVulkanApplicationInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    myImpl->myVulkanApplicationInfo.apiVersion = VK_API_VERSION_1_2;

    if (netero::isDebugMode && !VkUtils::CheckValidationLayerSupport()) {
        return RtCode::DEBUG_MISSING_VALIDATION_LAYERS;
    }

    VkInstanceCreateInfo createInfo {};
    auto                 extension = VkUtils::GetRequiredExtensions();
    createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    createInfo.pApplicationInfo = &myImpl->myVulkanApplicationInfo;
    const char** glfwExtensions = extension.data();
    createInfo.enabledExtensionCount = static_cast<uint32_t>(extension.size());
    createInfo.ppEnabledExtensionNames = glfwExtensions;
    createInfo.pNext = nullptr;
    if (netero::isDebugMode) {
        createInfo.enabledLayerCount = static_cast<uint32_t>(VkUtils::ValidationLayers.size());
        createInfo.ppEnabledLayerNames = VkUtils::ValidationLayers.data();
    }
    else { // ReSharper disable once CppUnreachableCode
        createInfo.enabledLayerCount = 0;
        createInfo.ppEnabledLayerNames = nullptr;
    }
    VkResult result = vkCreateInstance(&createInfo, nullptr, &myImpl->myVulkanInstance);
    if (result != VK_SUCCESS) {
        throw std::runtime_error("Failed to create instance, vkCode: " + std::to_string(result));
    }
    if (netero::isDebugMode) {
        VkDebugReportCallbackCreateInfoEXT reporterCreateInfo = {};
        VkUtils::PopulateDebugReportCreateInfo(reporterCreateInfo);
        result = VkUtils::CreateDebugReportEXT(myImpl->myVulkanInstance,
                                               &reporterCreateInfo,
                                               nullptr,
                                               &myImpl->myDebugReport);
        if (result != VK_SUCCESS) {
            return RtCode::DEBUG_FAILED_TO_SETUP_CALLBACKS;
        }
        VkDebugUtilsMessengerCreateInfoEXT messengerCreateInfo;
        VkUtils::PopulateDebugMessengerCreateInfo(messengerCreateInfo);
        result = VkUtils::CreateDebugMessengerEXT(myImpl->myVulkanInstance,
                                                  &messengerCreateInfo,
                                                  nullptr,
                                                  &myImpl->myDebugMessenger);
        if (result != VK_SUCCESS) {
            return RtCode::DEBUG_FAILED_TO_SETUP_CALLBACKS;
        }
    }
    return RtCode::SUCCESS;
}

void Application::Terminate()
{
    if (netero::isDebugMode) {
        VkUtils::DestroyDebugReportEXT(myImpl->myVulkanInstance, myImpl->myDebugReport, nullptr);
        VkUtils::DestroyDebugMessengerEXT(myImpl->myVulkanInstance,
                                          myImpl->myDebugMessenger,
                                          nullptr);
    }
    vkDestroyInstance(myImpl->myVulkanInstance, nullptr);
    glfwTerminate();
    myImpl.reset(nullptr);
}

std::pair<uint32_t, uint32_t> Application::GetScreenDimension()
{
    return { 0xFFFFFFFF, 0xFFFFFFFF };
}

Window* Application::CreateWindow(uint32_t aWidth, uint32_t anHeight, WindowMode aMode)
{
    return nullptr;
}

}; // namespace Netero::Gfx
