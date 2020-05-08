/**
 * Netero sources under BSD-3-Clause
 * see LICENSE.txt
 */

#include <iostream>
#include <netero/os.hpp>
#include <netero/graphics/application.hpp>
#include <netero/graphics/context.hpp>

const std::string g_triangleVertices_path = netero::os::getBundlePath() + "/shaders/triangleVertices.spv";
const std::string g_triangleFragment_path = netero::os::getBundlePath() + "/shaders/triangleFragment.spv";

int    main() {
    int rtCode = 0;
    std::cout << netero::os::getBundlePath() << std::endl;
    try {
        auto* app = netero::graphics::Application::Initialize("GraphicsExample");
        auto* context = app->newWindowedContext(800, 600, netero::graphics::WindowMode::FIX);
        std::cout << "Current Device:" << context->getCurrentPhysicalDeviceName() << std::endl;
        context->loadShader(g_triangleVertices_path, netero::graphics::ShaderStage::VERTEX);
        context->loadShader(g_triangleFragment_path, netero::graphics::ShaderStage::FRAGMENT);
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

