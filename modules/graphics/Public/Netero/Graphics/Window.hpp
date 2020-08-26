/**
 * Netero sources under BSD-3-Clause
 * see LICENSE.txt
 */

#pragma once

#include <cstdint>
#include <string>

#include <Netero/Graphics/Errors.hpp>

namespace Netero::Gfx {

enum class WindowMode : uint8_t {
    FIX = 0x01,
    BORDERLESS = 0x2,
    RESIZABLE = 0x4,
    FULLSCREEN = 0x8,
    WINDOWED = 0x10
};

class Window {
    public:
    virtual ~Window() = default;

    virtual GfxResult Show() = 0;
    virtual GfxResult Hide() = 0;
    virtual GfxResult Close() = 0;
    virtual GfxResult Update() = 0;
    virtual GfxResult PullEvent() = 0;

    virtual void                             SetTitle(const std::string&) = 0;
    [[nodiscard]] virtual const std::string& GetTitle() = 0;

    virtual void                   SetWidth(uint32_t) = 0;
    virtual void                   SetHeight(uint32_t) = 0;
    [[nodiscard]] virtual uint32_t GetWidth() = 0;
    [[nodiscard]] virtual uint32_t GetHeight() = 0;

    virtual void SetPosition(uint32_t anXAxis, uint32_t anYAxis) = 0;
};

} // namespace Netero::Gfx
