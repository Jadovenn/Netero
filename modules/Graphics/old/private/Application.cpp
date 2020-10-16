/**
 * Netero sources under BSD-3-Clause
 * see LICENSE.txt
 */

#include <stdexcept>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <netero/Netero.hpp>
#include <netero/graphics/application.hpp>
#include <netero/graphics/context.hpp>

#include "utils/vkUtils.hpp"

std::unique_ptr<netero::graphics::Application> netero::graphics::Application::s_instance = nullptr;

netero::graphics::Application*
netero::graphics::Application::Initialize(const std::string& ApplicationName)
{
    if (s_instance) {
        return s_instance.get();
    }
    s_instance = std::unique_ptr<Application>(new Application);
    s_instance->_appName = ApplicationName;
    glfwInit();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    s_instance->_vulkanAppInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    s_instance->_vulkanAppInfo.pApplicationName = s_instance->_appName.c_str();
    s_instance->_vulkanAppInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    s_instance->_vulkanAppInfo.pEngineName = "Netero";
    s_instance->_vulkanAppInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    s_instance->_vulkanAppInfo.apiVersion = VK_API_VERSION_1_2;

    VkInstanceCreateInfo createInfo = {};

    if (netero::isDebugMode && !vkUtils::checkValidationLayerSupport()) {
        throw std::runtime_error(
            "There are some missing validation layer, could not create render context");
    }
    // Setup create info struct for createInstance call
    auto extension = vkUtils::getRequiredExtensions();
    createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    createInfo.pApplicationInfo = &s_instance->_vulkanAppInfo;
    const char** glfwExtensions = extension.data();
    createInfo.enabledExtensionCount = static_cast<uint32_t>(extension.size());
    createInfo.ppEnabledExtensionNames = glfwExtensions;
    createInfo.pNext = nullptr;
    if (netero::isDebugMode) {
        createInfo.enabledLayerCount = static_cast<uint32_t>(vkUtils::validationLayers.size());
        createInfo.ppEnabledLayerNames = vkUtils::validationLayers.data();
    }
    else { // ReSharper disable once CppUnreachableCode
        createInfo.enabledLayerCount = 0;
        createInfo.ppEnabledLayerNames = nullptr;
    }
    VkResult result = vkCreateInstance(&createInfo, nullptr, &s_instance->_vulkanInstance);
    if (result != VK_SUCCESS) {
        throw std::runtime_error("Failed to create instance, vkCode: " + std::to_string(result));
    }
    if (netero::isDebugMode) {
        VkDebugReportCallbackCreateInfoEXT reporterCreateInfo = {};
        vkUtils::populateDebugReportCreateInfo(reporterCreateInfo);
        result = vkUtils::CreateDebugReportEXT(s_instance->_vulkanInstance,
                                               &reporterCreateInfo,
                                               nullptr,
                                               &s_instance->_debugReport);
        if (result != VK_SUCCESS) {
            throw std::runtime_error("Failed to setup debug report callback.");
        }
        VkDebugUtilsMessengerCreateInfoEXT messengerCreateInfo;
        vkUtils::populateDebugMessengerCreateInfo(messengerCreateInfo);
        result = vkUtils::CreateDebugMessengerEXT(s_instance->_vulkanInstance,
                                                  &messengerCreateInfo,
                                                  nullptr,
                                                  &s_instance->_debugMessenger);
        if (result != VK_SUCCESS) {
            throw std::runtime_error("Failed to setup debug messenger callback.");
        }
    }
    return s_instance.get();
}

netero::graphics::Application* netero::graphics::Application::GetInstance()
{
    return s_instance.get();
}

void netero::graphics::Application::Release()
{
    if (!s_instance) {
        return;
    }
    for (auto* context : s_instance->_activeContext) {
        delete context;
    }
    if (netero::isDebugMode) {
        vkUtils::DestroyDebugReportEXT(s_instance->_vulkanInstance,
                                       s_instance->_debugReport,
                                       nullptr);
        vkUtils::DestroyDebugMessengerEXT(s_instance->_vulkanInstance,
                                          s_instance->_debugMessenger,
                                          nullptr);
    }
    vkDestroyInstance(s_instance->_vulkanInstance, nullptr);
    glfwTerminate();
    s_instance.reset(nullptr);
}

netero::graphics::Context*
netero::graphics::Application::newWindowedContext(unsigned                     width,
                                                  unsigned                     height,
                                                  netero::graphics::WindowMode mode,
                                                  const std::string&           name,
                                                  const std::string&           deviceName)
{
    std::string contextName = name;
    if (contextName.empty()) {
        contextName = this->_appName;
    }
    auto* context = new (std::nothrow) netero::graphics::Context(this->_vulkanInstance,
                                                                 width,
                                                                 height,
                                                                 mode,
                                                                 contextName,
                                                                 deviceName);
    if (!context) {
        return nullptr;
    }
    this->_activeContext.push_back(context);
    return context;
}
