/**
 * Netero sources under BSD-3-Clause
 * see LICENSE.txt
 */

#include <memory>

#include <Vulkan/VkUtils.hpp>

#include <netero/netero.hpp>

#include <Application/ApplicationImpl.hpp>

namespace Netero::Gfx {

VulkanApplication::VulkanApplication(const std::string& anApplicationName)
    : myApplicationName(anApplicationName), myVulkanApplicationInfo {}, myVulkanInstance(nullptr)
{
}

VulkanApplication::~VulkanApplication() = default;

static std::unique_ptr<VulkanApplication> myImpl;

Application::RtCode Application::Initialize(const std::string& anApplicationName)
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
        return RtCode::DRIVER_CALL_FAILED;
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
    for (auto aWindow : myImpl->myWindows) {
        aWindow->Close();
        aWindow.reset();
    }
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
    if (!myImpl || aWidth <= 0 || anHeight <= 0) {
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
