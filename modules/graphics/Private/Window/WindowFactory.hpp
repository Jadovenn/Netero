/**
 * Netero sources under BSD-3-Clause
 * see LICENSE.txt
 */

#pragma once

#include <cassert>

#include <Vulkan/Vulkan.hpp>

#include <netero/patterns/IFactory.hpp>

#include <Window/Window_GLFW.hpp>

namespace Netero::Gfx {

class WindowFactory final
    : netero::patterns::IFactory<Window, VkInstance, int, int, WindowMode, const std::string&> {
    public:
    ~WindowFactory() final = default;

    std::shared_ptr<Window> Create(VkInstance         aVulkanInstance,
                                   int                aWidth,
                                   int                anHeight,
                                   WindowMode         aMode,
                                   const std::string& aTitle) final
    {
        std::shared_ptr<WindowGLFW> aWindow(
            new WindowGLFW(aVulkanInstance, aWidth, anHeight, aMode, aTitle));
        return aWindow;
    }
};

} // namespace Netero::Gfx
