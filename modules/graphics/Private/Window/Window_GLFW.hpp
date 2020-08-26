/**
 * Netero sources under BSD-3-Clause
 * see LICENSE.txt
 */

#pragma once

#include <string>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <Vulkan/Context/Context.hpp>

#include <Netero/Graphics/Window.hpp>

#include <Renderer/Renderer.hpp>
#include <Swapchain/Swapchain.hpp>

namespace Netero::Gfx {

class WindowFactory;

class WindowGLFW final: public Window {
    friend WindowFactory;

    WindowGLFW(VkInstance         aVulkanInstance,
               int                aWidth,
               int                anHeight,
               WindowMode         aMode,
               const std::string& aTitle);

    public:
    ~WindowGLFW() final;

    GfxResult Show() final;
    GfxResult Hide() final;
    GfxResult Close() final;
    GfxResult Update() final;
    GfxResult PullEvent() final;

    void               SetTitle(const std::string& aTitle) final { myTitle = aTitle; }
    const std::string& GetTitle() final { return myTitle; }

    void                   SetWidth(uint32_t aWidth) final { myWidth = aWidth; }
    void                   SetHeight(uint32_t anHeight) final { myHeight = anHeight; }
    [[nodiscard]] uint32_t GetWidth() final { return myWidth; }
    [[nodiscard]] uint32_t GetHeight() final { return myHeight; }

    void SetPosition(uint32_t anXAxis, uint32_t anYAxis);

    private:
    GfxResult PickPhysicalDevice();
    GfxResult CreateLogicalDevice();
    GfxResult CreateTransferQueue();

    int         myWidth;
    int         myHeight;
    WindowMode  myMode;
    std::string myTitle;

    GLFWwindow*  myWindow;
    Context      myContext;
    Swapchain    mySwapchain;
    RendererImpl myRenderer;
};

} // namespace Netero::Gfx
