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

std::vector<netero::graphics::Vertex>   triangle1_vertices = {
    {{0.f, 0.f}, {1.f, 0.f, 0.f}},
    {{0.5f, 1.f}, {0.f, 1.f, 0.f}},
    {{-0.5f, 1.f}, {0.f, 0.f, 1.f}}
};
std::vector<netero::graphics::Vertex>   triangle2_vertices = {
    {{-0.5f, -1.f}, {1.f, 1.f, 1.f}},
    {{0.f, -0.f}, {0.f, 1.f, 0.f}},
    {{-1.f, -0.f}, {0.f, 0.f, 1.f}}
};

std::vector<netero::graphics::Vertex>   triangle3_vertices = {
    {{0.5f, -1.f}, {1.f, 1.f, 1.f}},
    {{1.f, -0.f}, {0.f, 0.f, 1.f}},
    {{0.f, -0.f}, {1.f, 0.f, 0.f}},
};

int    main() {
    int rtCode = 0;
    try {
        auto* app = netero::graphics::Application::Initialize("Triangles");
        auto* context = app->newWindowedContext(800, 600, netero::graphics::WindowMode::RESIZABLE);
        std::cout << "Current Device:" << context->getCurrentPhysicalDeviceName() << std::endl;
        context->loadShader(g_triangleVertices_path, netero::graphics::ShaderStage::VERTEX);
        context->loadShader(g_triangleFragment_path, netero::graphics::ShaderStage::FRAGMENT);
        context->addVertices(triangle1_vertices);
        context->addVertices(triangle2_vertices);
        context->addVertices(triangle3_vertices);
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

