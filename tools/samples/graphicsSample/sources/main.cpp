/**
 * Netero sources under BSD-3-Clause
 * see LICENSE.txt
 */

#include <iostream>
#include <glm/glm.hpp>
#include <netero/os.hpp>
#include <netero/graphics/application.hpp>
#include <netero/graphics/context.hpp>
#include <netero/graphics/vertex.hpp>

const std::string g_triangleVertices_path = netero::os::getBundlePath() + "/shaders/triangleVertices.spv";
const std::string g_triangleFragment_path = netero::os::getBundlePath() + "/shaders/triangleFragment.spv";

std::vector<netero::graphics::Vertex>   vertices {
    {{-0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}},
    {{0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}},
    {{0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}},
    {{-0.5f, 0.5f}, {1.0f, 1.0f, 1.0f}}
};

std::vector<uint16_t>    indices{
    0, 1, 2, 2, 3, 0
};

int    main() {
    int rtCode = 0;
    try {
        auto* app = netero::graphics::Application::Initialize("Cube");
        auto* context = app->newWindowedContext(800, 600, netero::graphics::WindowMode::RESIZABLE);
        context->loadShader(g_triangleVertices_path, netero::graphics::ShaderStage::VERTEX);
        context->loadShader(g_triangleFragment_path, netero::graphics::ShaderStage::FRAGMENT);
        context->addVertices(vertices, indices);
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

