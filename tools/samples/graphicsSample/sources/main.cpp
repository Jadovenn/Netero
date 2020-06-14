/**
 * Netero sources under BSD-3-Clause
 * see LICENSE.txt
 */

#include <iostream>
#include <chrono>
#include <netero/os.hpp>
#include <netero/graphics/application.hpp>
#include <netero/graphics/context.hpp>
#include <netero/graphics/vertex.hpp>

const std::string g_3DObjectVertexShader_path = netero::os::getBundlePath() + "/shaders/ModelVertex.spv";
const std::string g_ColorFragmentShader_path = netero::os::getBundlePath() + "/shaders/ColorFragment.spv";
const std::string g_texture = netero::os::getBundlePath() + "/textures/cat.png";

std::vector<netero::graphics::Vertex>   squareVertices {
    {{-0.5f, -0.5f}, {1.f, 0.0f, 0.f}},
    {{0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}},
    {{0.5f, 0.5f}, {0.f, 0.0f, 1.f}},
    {{-0.5f, 0.5f}, {1.0f, 1.0f, 1.0f}}
};

std::vector<uint16_t>    squareIndices{
    0, 1, 2, 2, 3, 0
};

std::vector<netero::graphics::Vertex>   triangleVertices {
    {{0.f, -0.2f}, {1.f, 0.0f, 0.f}},
    {{0.25f, 0.25f}, {0.0f, 1.0f, 0.0f}},
    {{-0.25f, 0.25f}, {0.f, 0.0f, 1.f}},
};

struct GraphicSampleApp {
    netero::graphics::Application*  handle = nullptr;
    netero::graphics::Context*      context = nullptr;
    netero::graphics::Model*        square = nullptr;
    netero::graphics::Model*        triangle = nullptr;
    netero::graphics::Instance*     squareInstance1 = nullptr;
    netero::graphics::Instance*     squareInstance2 = nullptr;
    netero::graphics::Instance*     triangleInstance1 = nullptr;
};

void    RunApplication(GraphicSampleApp* myApp) {
    const auto startTime = std::chrono::high_resolution_clock::now();
    myApp->context->build();
    while (!myApp->context->shouldClose()) {
        auto now = std::chrono::high_resolution_clock::now();
        const float time = std::chrono::duration<float, std::chrono::seconds::period>(now - startTime).count();
        myApp->triangleInstance1->z.rotate(time * glm::radians(45.0f));
        myApp->squareInstance1->z.rotate(time * glm::radians(90.0f));
        myApp->squareInstance2->z.rotate(-1 * time * glm::radians(30.0f));
        myApp->context->update();
    }
    myApp->context->release();
}

void    InitializeApp(GraphicSampleApp* myApp) {
    myApp->handle = netero::graphics::Application::Initialize("GraphicsSample");
    myApp->context = myApp->handle->newWindowedContext(800, 600, netero::graphics::WindowMode::RESIZABLE);
    myApp->square = myApp->context->createModel();
    myApp->square->loadShader(g_3DObjectVertexShader_path, netero::graphics::ShaderStage::VERTEX);
    myApp->square->loadShader(g_ColorFragmentShader_path, netero::graphics::ShaderStage::FRAGMENT);
    myApp->square->addVertices(squareVertices, squareIndices);
    myApp->square->loadTexture(g_texture, netero::graphics::TextureSamplingMode::REPEAT);
    myApp->triangle = myApp->context->createModel();
    myApp->triangle->loadShader(g_3DObjectVertexShader_path, netero::graphics::ShaderStage::VERTEX);
    myApp->triangle->loadShader(g_ColorFragmentShader_path, netero::graphics::ShaderStage::FRAGMENT);
    myApp->triangle->addVertices(triangleVertices);
    myApp->squareInstance1 = myApp->square->createInstance();
    myApp->squareInstance1->x = 0.5;
    myApp->squareInstance1->y = -0.25;
    myApp->squareInstance1->x.scale(0.5);
    myApp->squareInstance1->y.scale(0.5);
    myApp->squareInstance2 = myApp->square->createInstance();
    myApp->squareInstance2->x = -0.5;
    myApp->squareInstance2->y = -0.25;
    myApp->squareInstance2->x.scale(0.5);
    myApp->squareInstance2->y.scale(0.5);
    myApp->triangleInstance1 = myApp->triangle->createInstance();
    myApp->triangleInstance1->y = 0.25;
    myApp->triangleInstance1->x.scale(2);
    myApp->triangleInstance1->y.scale(2);
}

void    CleanUpApp(GraphicSampleApp* myApp) {
    myApp->square->deleteInstance(myApp->squareInstance1);
    myApp->square->deleteInstance(myApp->squareInstance2);
    myApp->triangle->deleteInstance(myApp->triangleInstance1);
    myApp->context->deleteModel(myApp->square);
    myApp->context->deleteModel(myApp->triangle);
    netero::graphics::Application::Release();
}

#if defined(WIN_APP)
#include <windows.h>
#include <WinBase.h>
int WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {
#else
int    main() {
#endif
    int rtCode = 0;
    GraphicSampleApp    myApp{};
    try {
        InitializeApp(&myApp);
        RunApplication(&myApp);
        CleanUpApp(&myApp);
    }
    catch (const std::exception &e) {
        std::cerr << e.what() << std::endl;
        rtCode = 1;
    }
    catch(...) {
        std::cerr << "Fatal error." << std::endl;
        rtCode = 2;
    }
    return rtCode;
}

