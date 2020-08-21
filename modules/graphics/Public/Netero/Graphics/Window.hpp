/**
 * Netero sources under BSD-3-Clause
 * see LICENSE.txt
 */

#pragma once

#include <cstdint>
#include <string>

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

    enum class RtCode {
        SUCCESS,
        EXIT,
        DRIVER_CALL_ERROR,
        PHYSICAL_DEVICE_ERROR,
    };

    virtual RtCode Show() = 0;
    virtual RtCode Hide() = 0;
    virtual RtCode Close() = 0;
    virtual RtCode Update() = 0;
    virtual RtCode PullEvent() = 0;

    virtual void                             SetTitle(const std::string&) = 0;
    [[nodiscard]] virtual const std::string& GetTitle() = 0;

    virtual void                   SetWidth(uint32_t) = 0;
    virtual void                   SetHeight(uint32_t) = 0;
    [[nodiscard]] virtual uint32_t GetWidth() = 0;
    [[nodiscard]] virtual uint32_t GetHeight() = 0;

    virtual void SetPosition(uint32_t anXAxis, uint32_t anYAxis) = 0;
};

} // namespace Netero::Gfx
