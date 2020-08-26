/**
 * Netero sources under BSD-3-Clause
 * see LICENSE.txt
 */

#pragma once

#include <memory>
#include <utility>

#include <Netero/Graphics/Errors.hpp>
#include <Netero/Graphics/Window.hpp>

namespace Netero::Gfx {

class Application {
    public:
    virtual ~Application() = 0;

    static GfxResult Initialize(const std::string& anApplicationName);
    static void      Terminate();

    static std::pair<int, int> GetScreenDimension();

    static std::shared_ptr<Window>
                CreateWindow(int aWidth, int anHeight, WindowMode aMode, const std::string& aTitle = "");
    static void DestroyWindow(std::shared_ptr<Window>& aWindow);
};

} // namespace Netero::Gfx
