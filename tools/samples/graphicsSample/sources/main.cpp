/**
 * Netero sources under BSD-3-Clause
 * see LICENSE.txt
 */

#include <iostream>
#include <netero/os.hpp>
#include <netero/graphics/application.hpp>
#include <netero/graphics/context.hpp>
#include <netero/graphics/vertex.hpp>

const std::string g_triangleVertices_path = netero::os::getBundlePath() + "/shaders/triangleVertices.spv";
const std::string g_triangleFragment_path = netero::os::getBundlePath() + "/shaders/triangleFragment.spv";
const std::string g_texture = netero::os::getBundlePath() + "/textures/cat.png";

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
        // Initialize the graphics module with the name of the app
        auto* app = netero::graphics::Application::Initialize("GraphicsSample");
        // Create a new context with its window
        auto* context = app->newWindowedContext(800, 600, netero::graphics::WindowMode::RESIZABLE);
        // Create a new model that is a square
        auto* squareModel = context->createModel();
        squareModel->loadShader(g_triangleVertices_path, netero::graphics::ShaderStage::VERTEX);
        squareModel->loadShader(g_triangleFragment_path, netero::graphics::ShaderStage::FRAGMENT);
        squareModel->addVertices(vertices, indices);
        // Create an instance of our model that is rendered in the graphic pipeline
        auto* myFirstSquare = squareModel->createInstance();
        context->run();
        squareModel->deleteInstance(myFirstSquare);
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

