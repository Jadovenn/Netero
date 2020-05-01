/**
 * Netero sources under BSD-3-Clause
 * see LICENSE.txt
 */

#pragma once

#include <netero/graphics/renderContext.hpp>
#include <string>
#include <memory>

namespace netero::graphics {

    enum class WindowMode {
        FIX,
        RESIZABLE,
        FULLSCREEN,
        WINDOWED,
    };

    class window {
    public:
        friend RenderContext;
        window(unsigned width, unsigned height, const std::string& name);
        ~window();

        void    initialize() const;
        void    runLoop() const;
        
    protected:
        class impl;
        std::unique_ptr<impl>    _pImpl;
    };
}

