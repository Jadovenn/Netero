/**
 * Netero sources under BSD-3-Clause
 * see LICENSE.txt
 */

#pragma once

#include <memory>
#include <vector>
#include <string>
#include <vulkan/vulkan.h>
#include <netero/graphics/window.hpp>
#include <netero/graphics/context.hpp>

namespace netero::graphics {

    class Application {
        Application() = default;
        static std::unique_ptr<Application>    s_instance;
    public:
        Application(const Application&) = delete;
        Application(Application&&) = delete;
        Application& operator=(const Application&) = delete;
        Application& operator=(Application&&) = delete;
        ~Application() = default;

        static Application* Initialize(const std::string&);
        static Application* GetInstance();
        static void         Release();

        Context* newWindowedContext(unsigned width,
            unsigned height,
            WindowMode mode = WindowMode::FIX,
            const std::string& deviceName = "",
            const std::string& name = "");

    private:
        std::string       _appName;
        VkInstance        _vulkanInstance = nullptr;
        VkApplicationInfo _vulkanAppInfo = {};
        VkDebugUtilsMessengerEXT    _debugMessenger = nullptr;  // NOLINT(clang-diagnostic-unused-private-field)
        VkDebugReportCallbackEXT    _debugReport = nullptr;  // NOLINT(clang-diagnostic-unused-private-field)
        std::vector<Context*>       _activeContext;
    };
}

