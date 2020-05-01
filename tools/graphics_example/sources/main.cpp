/**
 * Netero sources under BSD-3-Clause
 * see LICENSE.txt
 */

#include <iostream>
#include <netero/graphics/application.hpp>
#include <netero/graphics/context.hpp>
#include <netero/graphics/physicalDevice.hpp>

void    displayAvailablePhysicalDevice() {
    const auto physicalDevice = netero::graphics::PhysicalDevice::Instance();
    const auto physicalDevices = physicalDevice->getDevices();
    
    for (const auto& device: physicalDevices) {
        std::cout << "Score: " << device.first << " name: " << device.second << std::endl;
    }
}

int    main() {
    int rtCode = 0;
    try {
        auto* app = netero::graphics::Application::Initialize("GraphicsExample");
        auto* context = app->newWindowedContext(800, 600, netero::graphics::WindowMode::FIX);
        displayAvailablePhysicalDevice();
        context->run();
    }
    catch (const std::exception &e) {
        std::cerr << e.what() << std::endl;
        rtCode = 1;
    }
    catch(...) {
        std::cerr << "Fatal error." << std::endl;
        rtCode = 0;
    }
    netero::graphics::Application::Release();
    return rtCode;
}

