/**
 * Netero sources under BSD-3-Clause
 * see LICENSE.txt
 */

#include <iostream>
#include <netero/graphics/application.hpp>
#include <netero/graphics/context.hpp>

int    main() {
    int rtCode = 0;
    try {
        auto* app = netero::graphics::Application::Initialize("GraphicsExample");
        auto* context = app->newWindowedContext(800, 600, netero::graphics::WindowMode::FIX);
        std::cout << "Current Device:" << context->getCurrentPhysicalDeviceName() << std::endl;
        context->run();
    }
    catch (const std::exception &e) {
        std::cerr << e.what() << std::endl;
        rtCode = 1;
    }
    catch(...) {
        std::cerr << "Fatal error." << std::endl;
        rtCode = 2;
    }
    netero::graphics::Application::Release();
    return rtCode;
}

