/**
 * Netero sources under BSD-3-Clause
 * see LICENSE.txt
 */

#pragma once

#include <string>

#include <GLFW/glfw3.h>

#include <Netero/Graphics/Window.hpp>

namespace Netero::Gfx {

class WindowFactory;

class WindowGLFW final: public Window {
    friend WindowFactory;

    WindowGLFW(uint32_t aWidth, uint32_t anHeight, WindowMode aMode, const std::string& aTitle);

    public:
    ~WindowGLFW() final = default;

    void               SetTitle(const std::string& aTitle) final { myTitle = aTitle; }
    const std::string& GetTitle() final { return myTitle; }

    void                   SetWidth(uint32_t aWidth) final { myWidth = aWidth; }
    void                   SetHeight(uint32_t anHeight) final { myHeight = anHeight; }
    [[nodiscard]] uint32_t GetWidth() final { return myWidth; }
    [[nodiscard]] uint32_t GetHeight() final { return myHeight; }

    void SetPosition(uint32_t anXAxis, uint32_t anYAxis);

    private:
    uint32_t    myWidth;
    uint32_t    myHeight;
    WindowMode  myMode;
    std::string myTitle;

    GLFWwindow* myWindow;
};

} // namespace Netero::Gfx
