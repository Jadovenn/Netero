/**
 * Netero sources under BSD-3-Clause
 * see LICENSE.txt
 */

#pragma once

#include <memory>
#include <utility>

#include <Netero/Graphics/Window.hpp>

namespace Netero::Gfx {

class Application {
    public:
    virtual ~Application() = 0;

    enum class RtCode {
        SUCCESS,
        GFX_NOT_INITIALIZED,
        DRIVER_CALL_FAILED,
        DEBUG_MISSING_VALIDATION_LAYERS,
        DEBUG_FAILED_TO_SETUP_CALLBACKS
    };

    static RtCode Initialize(const std::string& anApplicationName);
    static void   Terminate();

    static std::pair<int, int> GetScreenDimension();

    static std::shared_ptr<Window>
                CreateWindow(int aWidth, int anHeight, WindowMode aMode, const std::string& aTitle = "");
    static void DestroyWindow(std::shared_ptr<Window>& aWindow);
};

} // namespace Netero::Gfx
