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
        DEBUG_MISSING_VALIDATION_LAYERS,
        DEBUG_FAILED_TO_SETUP_CALLBACKS
    };

    static RtCode Initialize(const std::string& anApplicationName);
    static void Terminate();

    static std::pair<uint32_t, uint32_t> GetScreenDimension();

    static Window* CreateWindow(uint32_t aWidth, uint32_t anHeight, WindowMode aMode);

    private:
    class Impl;
    static std::unique_ptr<Impl> myImpl;
};

} // namespace Netero::Gfx
