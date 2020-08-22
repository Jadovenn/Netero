/**
 * Netero sources under BSD-3-Clause
 * see LICENSE.txt
 */

#include <set>

#include <Vulkan/VkUtils.hpp>

#include <netero/logger.hpp>
#include <netero/netero.hpp>

#include <Window/Window_GLFW.hpp>

namespace Netero::Gfx {

WindowGLFW::WindowGLFW(VkInstance         aVulkanInstance,
                       int                aWidth,
                       int                anHeight,
                       WindowMode         aMode,
                       const std::string& aTitle)

    : myWidth(aWidth),
      myHeight(anHeight),
      myMode(aMode),
      myTitle(aTitle),
      myWindow(nullptr),
      mySwapchain(myContext, 2),
      myRenderer(myContext)
{
    myContext.myVulkanInstance = aVulkanInstance;
    myContext.myWidth = myWidth;
    myContext.myHeight = myHeight;

    GLFWmonitor* monitor = nullptr;

    if (myMode == WindowMode::FIX) {
        glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
    }
    else if (myMode == WindowMode::BORDERLESS) {
        glfwWindowHint(GLFW_DECORATED, GLFW_FALSE);
    }
    else if (myMode == WindowMode::FULLSCREEN) {
        monitor = glfwGetPrimaryMonitor();
        glfwGetMonitorWorkarea(monitor, nullptr, nullptr, &myWidth, &myHeight);
    }

    myWindow = glfwCreateWindow(myWidth, myHeight, myTitle.c_str(), monitor, nullptr);

    if (myMode == WindowMode::BORDERLESS) {
        monitor = glfwGetPrimaryMonitor();
        int wHeight, wWidth = 0;
        glfwGetMonitorWorkarea(monitor, nullptr, nullptr, &wWidth, &wHeight);
        wWidth /= 2;
        wWidth -= myWidth / 2;
        wHeight /= 2;
        wHeight -= myHeight / 2;
        glfwSetWindowPos(myWindow, wWidth, wHeight);
    }
}

WindowGLFW::~WindowGLFW()
{
}

Window::RtCode WindowGLFW::Show()
{
    if (!myContext.mySurface) {
        const VkResult result = glfwCreateWindowSurface(myContext.myVulkanInstance,
                                                        myWindow,
                                                        nullptr,
                                                        &myContext.mySurface);
        if (result != VK_SUCCESS) {
            return RtCode::DRIVER_CALL_ERROR;
        }
        if (PickPhysicalDevice() != RtCode::SUCCESS) {
            return RtCode::DRIVER_CALL_ERROR;
        }
        if (CreateLogicalDevice() != RtCode::SUCCESS) {
            return RtCode::DRIVER_CALL_ERROR;
        }
        if (CreateTransferQueue() != RtCode::SUCCESS) {
            return RtCode::DRIVER_CALL_ERROR;
        }
        mySwapchain.Initialize();
        myRenderer.Initialize();
        mySwapchain.Build();
        myRenderer.Build();
    }
    glfwShowWindow(myWindow);
    return RtCode::SUCCESS;
}

Window::RtCode WindowGLFW::Close()
{
    if (myContext.mySurface) {
        myRenderer.Teardown();
        mySwapchain.Teardown();
        vkDestroySurfaceKHR(myContext.myVulkanInstance, myContext.mySurface, nullptr);
        vkDestroyCommandPool(myContext.myLogicalDevice, myContext.myTransferCommandPool, nullptr);
        vkDestroyDevice(myContext.myLogicalDevice, nullptr);
        glfwDestroyWindow(myWindow);
    }
    return RtCode::SUCCESS;
}

Window::RtCode WindowGLFW::Hide()
{
    glfwHideWindow(myWindow);
    return RtCode::SUCCESS;
}

Window::RtCode WindowGLFW::Update()
{
    Frame aFrame {};
    assert(myContext.myLogicalDevice != nullptr); // Do you have call Show() before update?
    assert(myContext.mySurface != nullptr);
    Swapchain::RtCode result = mySwapchain.PrepareFrame(aFrame);
    if (result == Swapchain::RtCode::OUT_OF_DATE_SWAPCHAIN) {
        vkDeviceWaitIdle(myContext.myLogicalDevice);
        glfwGetFramebufferSize(myWindow, &myContext.myWidth, &myContext.myHeight);
        while (myContext.myWidth == 0 || myContext.myHeight == 0) {
            glfwGetFramebufferSize(myWindow, &myContext.myWidth, &myContext.myHeight);
            glfwWaitEvents();
        }
        mySwapchain.ReBuild();
        myRenderer.ReBuild();
        return RtCode::SUCCESS;
    }
    else if (result != Swapchain::RtCode::SUCCESS) {
        return RtCode::DRIVER_CALL_ERROR;
    }
    if (myRenderer.Update() != RendererImpl::RtCode::SUCCESS) {
        return RtCode::DRIVER_CALL_ERROR;
    }
    result = mySwapchain.SubmitFrame(aFrame);
    if (result == Swapchain::RtCode::OUT_OF_DATE_SWAPCHAIN) {
        mySwapchain.ReBuild();
        myRenderer.ReBuild();
        return RtCode::SUCCESS;
    }
    else if (result != Swapchain::RtCode::SUCCESS) {
        return RtCode::DRIVER_CALL_ERROR;
    }
    return RtCode::SUCCESS;
}

Window::RtCode WindowGLFW::PullEvent()
{
    glfwPollEvents();
    if (glfwWindowShouldClose(myWindow)) {
        return RtCode::EXIT;
    }
    return RtCode::SUCCESS;
}

void WindowGLFW::SetPosition(uint32_t anXAxis, uint32_t anYAxis)
{
}

Window::RtCode WindowGLFW::PickPhysicalDevice()
{
    myContext.myPhysicalDevice =
        VkUtils::GetBestDevice(myContext.myVulkanInstance, myContext.mySurface);
    if (!myContext.myPhysicalDevice) {
        LOG_ERROR << "No suitable physical device found!" << std::endl;
        return RtCode::PHYSICAL_DEVICE_ERROR;
    }
    if (!VkUtils::CheckDeviceSuitable(myContext.myPhysicalDevice,
                                      VkUtils::DefaultDeviceExtensions)) {
        LOG_ERROR << "The device \'" << VkUtils::GetDeviceName(myContext.myPhysicalDevice)
                  << "\' is not suitable for windowed context." << std::endl;
        return RtCode::PHYSICAL_DEVICE_ERROR;
    }
    const auto swapChainDetails =
        VkUtils::QuerySwapChainSupport(myContext.myPhysicalDevice, myContext.mySurface);
    if (swapChainDetails.formats.empty() || swapChainDetails.presentMode.empty()) {
        LOG_ERROR << "The device \'" << VkUtils::GetDeviceName(myContext.myPhysicalDevice)
                  << "\' has no suitable swapchain." << std::endl;
        return RtCode::PHYSICAL_DEVICE_ERROR;
    }
    return RtCode::SUCCESS;
}

Window::RtCode WindowGLFW::CreateLogicalDevice()
{
    auto indices = VkUtils::FindQueueFamilies(myContext.myPhysicalDevice, myContext.mySurface);
    VkPhysicalDeviceFeatures deviceFeatures {};
    deviceFeatures.samplerAnisotropy = VK_TRUE;
    VkDeviceCreateInfo deviceCreateInfo {};

    float                                queuePriority = 1.f;
    std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
    std::set<uint32_t>                   uniqueQueueFamilies = { indices.graphicsFamily.value(),
                                               indices.presentFamily.value() };
    for (auto queueFamily : uniqueQueueFamilies) {
        VkDeviceQueueCreateInfo queueCreateInfo = {};
        queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queueCreateInfo.queueFamilyIndex = queueFamily;
        queueCreateInfo.queueCount = 1;
        queueCreateInfo.pQueuePriorities = &queuePriority;
        queueCreateInfos.push_back(queueCreateInfo);
    }

    deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    deviceCreateInfo.pQueueCreateInfos = queueCreateInfos.data();
    deviceCreateInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
    deviceCreateInfo.pEnabledFeatures = &deviceFeatures;
    deviceCreateInfo.enabledExtensionCount =
        static_cast<uint32_t>(VkUtils::DefaultDeviceExtensions.size());
    deviceCreateInfo.ppEnabledExtensionNames = VkUtils::DefaultDeviceExtensions.data();
    if (netero::isDebugMode) {
        deviceCreateInfo.enabledLayerCount =
            static_cast<uint32_t>(VkUtils::ValidationLayers.size());
        deviceCreateInfo.ppEnabledLayerNames = VkUtils::ValidationLayers.data();
    }
    // ReSharper disable once CppUnreachableCode
    else {
        deviceCreateInfo.enabledLayerCount = 0;
    }
    const VkResult result = vkCreateDevice(myContext.myPhysicalDevice,
                                           &deviceCreateInfo,
                                           nullptr,
                                           &myContext.myLogicalDevice);
    if (result != VK_SUCCESS) {
        return RtCode::DRIVER_CALL_ERROR;
    }
    vkGetDeviceQueue(myContext.myLogicalDevice,
                     indices.graphicsFamily.value(),
                     0,
                     &myContext.myGraphicsQueue);
    vkGetDeviceQueue(myContext.myLogicalDevice,
                     indices.presentFamily.value(),
                     0,
                     &myContext.myPresentQueue);
    if (indices.transferFamily.has_value()) {
        vkGetDeviceQueue(myContext.myLogicalDevice,
                         indices.transferFamily.value(),
                         0,
                         &myContext.myTransferQueue);
    }
    else {
        myContext.myTransferQueue = myContext.myPresentQueue;
    }
    return RtCode::SUCCESS;
}

Window::RtCode WindowGLFW::CreateTransferQueue()
{
    VkUtils::QueueFamilyIndices queueFamilyIndices =
        VkUtils::FindQueueFamilies(myContext.myPhysicalDevice, myContext.mySurface);

    VkCommandPoolCreateInfo poolInfo {};
    poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    if (queueFamilyIndices.transferFamily.has_value()) {
        poolInfo.queueFamilyIndex = queueFamilyIndices.transferFamily.value();
    }
    else {
        poolInfo.queueFamilyIndex = queueFamilyIndices.graphicsFamily.value();
    }
    poolInfo.flags = VK_COMMAND_POOL_CREATE_TRANSIENT_BIT;
    if (vkCreateCommandPool(myContext.myLogicalDevice,
                            &poolInfo,
                            nullptr,
                            &myContext.myTransferCommandPool) != VK_SUCCESS) {
        return RtCode::DRIVER_CALL_ERROR;
    }
    return RtCode::SUCCESS;
}

} // namespace Netero::Gfx
