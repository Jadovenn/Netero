/**
 * Netero sources under BSD-3-Clause
 * see LICENSE.txt
 */

#include <Window/Window_GLFW.hpp>

namespace Netero::Gfx {

WindowGLFW::WindowGLFW(VkInstance         aVulkanInstance,
                       int                aWidth,
                       int                anHeight,
                       WindowMode         aMode,
                       const std::string& aTitle)

    : myWidth(aWidth), myHeight(anHeight), myMode(aMode), myTitle(aTitle), myWindow(nullptr)
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
    vkDestroySurfaceKHR(myContext.myVulkanInstance, myContext.mySurface, nullptr);
    glfwDestroyWindow(myWindow);
}

Window::RtCode WindowGLFW::Show() {
    const VkResult result = glfwCreateWindowSurface(myContext.myVulkanInstance, myWindow, nullptr, &myContext.mySurface);
    if (result != VK_SUCCESS) {
        return RtCode::DRIVER_CALL_ERROR;
    }
    return RtCode::SUCCESS;
}

Window::RtCode WindowGLFW::Update()
{
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

} // namespace Netero::Gfx
