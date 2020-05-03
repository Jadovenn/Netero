/**
 * Netero sources under BSD-3-Clause
 * see LICENSE.txt
 */

#pragma once

#include <string>
#include <memory>
#include <vector>
#include <vulkan/vulkan.h>
#include <netero/graphics/window.hpp>

namespace netero::graphics {

    class Application;

    class Context {
        Context(VkInstance vulkanInstance,
            unsigned width,
            unsigned height,
            WindowMode mode,
            const std::string& name,
            const std::string& deviceName);
    public:
        friend Application;
        Context(Context&&) = delete;
        Context(const Context&) = delete;
        Context& operator=(const Context&) = delete;
        Context& operator=(Context&&) = delete;
        ~Context();

        // Context related
        void    run() const;

        // Device Related
        [[nodiscard]] std::vector<std::string>  getPhysicalDevices() const;
        [[nodiscard]] std::string   getCurrentPhysicalDeviceName() const;

    private:
        void setPhysicalDevice(VkPhysicalDevice device);

        VkInstance  _vulkanInstance;
        unsigned    _height;
        unsigned    _width;
        WindowMode  _windowMode;
        const std::string   _name;
        const std::string   _deviceName;

        VkPhysicalDevice    _physicalDevice = nullptr;
        VkDevice            _logicalDevice = nullptr;
        VkQueue             _graphicsQueue = nullptr;
        VkQueue             _presentQueue = nullptr;
        VkSurfaceKHR        _surface = nullptr;
        struct impl;
        std::unique_ptr<impl>   _pImpl;
    };
}

