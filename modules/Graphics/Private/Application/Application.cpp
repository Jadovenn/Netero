/**
 * Netero sources under BSD-3-Clause
 * see LICENSE.txt
 */

#include <memory>
#include <set>

#include <Vulkan/VkUtils.hpp>

#include <Netero/Logger.hpp>
#include <Netero/Netero.hpp>

#include <Application/ApplicationImpl.hpp>

namespace Netero::Gfx {

VulkanApplication::VulkanApplication(const std::string& anApplicationName)
    : myApplicationName(anApplicationName), myVulkanApplicationInfo {}
{
}

VulkanApplication::~VulkanApplication() = default;

static std::unique_ptr<VulkanApplication> myImpl;

VkInstance VulkanApplication::GetInstance()
{
    assert(myImpl);
    return myImpl->myVulkanInstance;
}

GfxResult Application::Initialize(const std::string& anApplicationName)
{
    myImpl = std::make_unique<VulkanApplication>(anApplicationName);
    glfwInit();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    myImpl->myVulkanApplicationInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    myImpl->myVulkanApplicationInfo.pApplicationName = myImpl->myApplicationName.c_str();
    myImpl->myVulkanApplicationInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    myImpl->myVulkanApplicationInfo.pEngineName = "Netero";
    myImpl->myVulkanApplicationInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    myImpl->myVulkanApplicationInfo.apiVersion = VK_API_VERSION_1_2;

    if (Netero::IsDebugMode && !VkUtils::CheckValidationLayerSupport()) {
        return GfxResult::DEBUG_MISSING_VALIDATION_LAYERS;
    }

    VkInstanceCreateInfo createInfo {};
    auto                 extension = VkUtils::GetRequiredExtensions();
    createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    createInfo.pApplicationInfo = &myImpl->myVulkanApplicationInfo;
    const char** glfwExtensions = extension.data();
    createInfo.enabledExtensionCount = static_cast<uint32_t>(extension.size());
    createInfo.ppEnabledExtensionNames = glfwExtensions;
    createInfo.pNext = nullptr;

    if (Netero::IsDebugMode) {
        createInfo.enabledLayerCount = static_cast<uint32_t>(VkUtils::ValidationLayers.size());
        createInfo.ppEnabledLayerNames = VkUtils::ValidationLayers.data();
    }
    else { // ReSharper disable once CppUnreachableCode
        createInfo.enabledLayerCount = 0;
        createInfo.ppEnabledLayerNames = nullptr;
    }

    VkResult result = vkCreateInstance(&createInfo, nullptr, &myImpl->myVulkanInstance);
    if (result != VK_SUCCESS) {
        return GfxResult::DRIVER_CALL_FAILED;
    }

    if (Netero::IsDebugMode) {
        VkDebugReportCallbackCreateInfoEXT reporterCreateInfo = {};
        VkUtils::PopulateDebugReportCreateInfo(reporterCreateInfo);
        result = VkUtils::CreateDebugReportEXT(myImpl->myVulkanInstance,
                                               &reporterCreateInfo,
                                               nullptr,
                                               &myImpl->myDebugReport);
        if (result != VK_SUCCESS) {
            return GfxResult::DEBUG_FAILED_TO_SETUP_CALLBACKS;
        }

        VkDebugUtilsMessengerCreateInfoEXT messengerCreateInfo;
        VkUtils::PopulateDebugMessengerCreateInfo(messengerCreateInfo);
        result = VkUtils::CreateDebugMessengerEXT(myImpl->myVulkanInstance,
                                                  &messengerCreateInfo,
                                                  nullptr,
                                                  &myImpl->myDebugMessenger);
        if (result != VK_SUCCESS) {
            return GfxResult::DEBUG_FAILED_TO_SETUP_CALLBACKS;
        }
    }

    return GfxResult::SUCCESS;
}

void Application::Terminate()
{
    for (auto aWindow : myImpl->myWindows) {
        aWindow->Close();
        aWindow.reset();
    }
    if (Netero::IsDebugMode) {
        VkUtils::DestroyDebugReportEXT(myImpl->myVulkanInstance, myImpl->myDebugReport, nullptr);
        VkUtils::DestroyDebugMessengerEXT(myImpl->myVulkanInstance,
                                          myImpl->myDebugMessenger,
                                          nullptr);
    }
    vkDestroyInstance(myImpl->myVulkanInstance, nullptr);
    glfwTerminate();
    myImpl.reset(nullptr);
}

std::pair<int, int> Application::GetScreenDimension()
{
    GLFWmonitor*        aMonitor = glfwGetPrimaryMonitor();
    std::pair<int, int> screenDimension;
    glfwGetMonitorWorkarea(aMonitor,
                           nullptr,
                           nullptr,
                           &screenDimension.first,
                           &screenDimension.second);
    return screenDimension;
}

std::shared_ptr<Window>
Application::CreateWindow(int aWidth, int anHeight, WindowMode aMode, const std::string& aTitle)
{
    assert(myImpl);
    if (aWidth <= 0 || anHeight <= 0) {
        return nullptr;
    }
    std::shared_ptr<Window> aWindow;
    if (aTitle.empty()) {
        aWindow = myImpl->myWindowFactory.Create(myImpl->myVulkanInstance,
                                                 aWidth,
                                                 anHeight,
                                                 aMode,
                                                 myImpl->myApplicationName);
    }
    else {
        aWindow = myImpl->myWindowFactory.Create(myImpl->myVulkanInstance,
                                                 aWidth,
                                                 anHeight,
                                                 aMode,
                                                 aTitle);
    }
    myImpl->myWindows.push_back(aWindow);
    return aWindow;
}

void Application::DestroyWindow(std::shared_ptr<Window>& aWindow)
{
    auto it = std::find(myImpl->myWindows.begin(), myImpl->myWindows.end(), aWindow);
    if (it != myImpl->myWindows.end()) {
        myImpl->myWindows.erase(it);
        aWindow->Close();
        aWindow.reset();
    }
}

}; // namespace Netero::Gfx
