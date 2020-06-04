/**
 * Netero sources under BSD-3-Clause
 * see LICENSE.txt
 */

#include <iostream>
#include <netero/os.hpp>
#include <netero/graphics/application.hpp>
#include <netero/graphics/context.hpp>
#include <netero/graphics/vertex.hpp>

const std::string g_2DObjectVertexShader_path = netero::os::getBundlePath() + "/shaders/2DObjectProj.spv";
const std::string g_3DObjectVertexShader_path = netero::os::getBundlePath() + "/shaders/3DObjectProj.spv";
const std::string g_ColorFragmentShader_path = netero::os::getBundlePath() + "/shaders/ColorFragment.spv";
const std::string g_texture = netero::os::getBundlePath() + "/textures/cat.png";

std::vector<netero::graphics::Vertex>   vertices {
    {{-0.5f, -0.5f}, {1.f, 0.0f, 0.f}},
    {{0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}},
    {{0.5f, 0.5f}, {0.f, 0.0f, 1.f}},
    {{-0.5f, 0.5f}, {1.0f, 1.0f, 1.0f}}
};


std::vector<netero::graphics::Vertex>   triangleVertices {
    {{-1.f, -1.f}, {1.f, 0.0f, 0.f}},
    {{-1.f, -0.5f}, {0.f, 1.0f, 0.f}},
    {{-0.5f, -0.5f}, {0.f, 0.0f, 1.f}},
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
        auto* square2DModel = context->createModel();
        square2DModel->loadShader(g_3DObjectVertexShader_path, netero::graphics::ShaderStage::VERTEX);
        square2DModel->loadShader(g_ColorFragmentShader_path, netero::graphics::ShaderStage::FRAGMENT);
        square2DModel->addVertices(vertices, indices);
        // Create an instance of our model that is rendered in the graphic pipeline
        auto* mySquare = square2DModel->createInstance();
        auto* mySecondSquare = square2DModel->createInstance();
        mySquare->z.rotate(glm::radians(45.0f));
        mySquare->x.scale(0.5);
        mySquare->y.scale(0.5);
        context->run();
        square2DModel->deleteInstance(mySquare);
        square2DModel->deleteInstance(mySecondSquare);
        context->deleteModel(square2DModel);
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

