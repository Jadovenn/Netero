/**
 * Netero sources under BSD-3-Clause
 * see LICENSE.txt
 */

#pragma once

#include <string>
#include <memory>
#include <vulkan/vulkan.h>
#include <netero/netero.hpp>

namespace netero::graphics {

    class window;

    class RenderContext {
        explicit RenderContext(const std::string &appName, bool enableValidationLayers = netero::isDebugMode);
        static std::unique_ptr<RenderContext>  _instance;
        void    initialize() const;
        void    release() const;
    public:
        static void             Initialize(const std::string& appName, bool enableValidationLayers = netero::isDebugMode);
        static RenderContext*   Instance();
        static void             Release();
        RenderContext(const RenderContext&) = delete;
        RenderContext(RenderContext&&) = delete;
        RenderContext& operator=(const RenderContext&) = delete;
        RenderContext& operator=(RenderContext&&) = delete;
        ~RenderContext();

        void    setPhysicalDevice(VkPhysicalDevice device) const;
        void    setTargetWindow(netero::graphics::window*) const;

    private:
        class impl;
        std::unique_ptr<impl>    pImpl;
    };
}

