/**
 * Netero sources under BSD-3-Clause
 * see LICENSE.txt
 */

#include <chrono>
#include <iostream>

#include <netero/extra/tiny_obj_loader.hpp>
#include <netero/graphics/application.hpp>
#include <netero/graphics/context.hpp>
#include <netero/graphics/instance.hpp>
#include <netero/graphics/vertex.hpp>
#include <netero/os.hpp>

const std::string VikingHouseVertexShaderPath =
    netero::os::getBundlePath() + "/shaders/VikingHouseVertexShader.spv";
const std::string VikingHouseFragmentShaderPath =
    netero::os::getBundlePath() + "/shaders/VikingHouseFragmentShader.spv";
const std::string VikingHouseModelPath =
    netero::os::getBundlePath() + "/resources/VikingHouseAsset/viking_house.OBJ";
const std::string VikingHouseTexturePath =
    netero::os::getBundlePath() + "/resources/VikingHouseAsset/viking_house.png";

struct GraphicSampleApp {
    netero::graphics::Application* handle = nullptr;
    netero::graphics::Context*     context = nullptr;
    netero::graphics::Model*       vikingHouse = nullptr;
    netero::graphics::Instance*    vikingHouseInstance = nullptr;
};

void RunApplication(GraphicSampleApp* myApp)
{
    const auto startTime = std::chrono::high_resolution_clock::now();
    myApp->context->build();
    while (!myApp->context->shouldClose()) {
        auto        now = std::chrono::high_resolution_clock::now();
        const float time =
            std::chrono::duration<float, std::chrono::seconds::period>(now - startTime).count();
        myApp->vikingHouseInstance->z.rotate((std::sin(time * glm::radians(30.f)) - 0.80f) * 0.85f);
        myApp->context->update();
    }
    myApp->context->release();
}

void InitializeApp(GraphicSampleApp* myApp)
{
    myApp->handle = netero::graphics::Application::Initialize("Netero Viking House");
    myApp->context =
        myApp->handle->newWindowedContext(800, 600, netero::graphics::WindowMode::RESIZABLE);
    myApp->vikingHouse = myApp->context->createModel();
    myApp->vikingHouse->loadShader(VikingHouseVertexShaderPath,
                                   netero::graphics::ShaderStage::VERTEX);
    myApp->vikingHouse->loadShader(VikingHouseFragmentShaderPath,
                                   netero::graphics::ShaderStage::FRAGMENT);
    myApp->vikingHouse->loadTexture(VikingHouseTexturePath,
                                    netero::graphics::TextureSamplingMode::REPEAT);
    tinyobj::attrib_t                     attrib;
    std::vector<netero::graphics::Vertex> vertices;
    std::vector<tinyobj::shape_t>         shapes;
    std::vector<tinyobj::material_t>      materials;
    std::string                           warn, err;
    if (!tinyobj::LoadObj(&attrib,
                          &shapes,
                          &materials,
                          &warn,
                          &err,
                          VikingHouseModelPath.c_str())) {
        throw std::runtime_error(warn + err);
    }
    for (const auto& shape : shapes) {
        for (const auto& index : shape.mesh.indices) {
            netero::graphics::Vertex vertex {};
            vertex.pos = { attrib.vertices[3 * index.vertex_index + 0],
                           attrib.vertices[3 * index.vertex_index + 1],
                           attrib.vertices[3 * index.vertex_index + 2] };
            vertex.texCord = { attrib.texcoords[2 * index.texcoord_index + 0],
                               1.0f - attrib.texcoords[2 * index.texcoord_index + 1] };
            vertex.color = { 1.0f, 1.0f, 1.0f };
            vertices.push_back(vertex);
        }
    }
    myApp->vikingHouse->addVertices(vertices);
    myApp->vikingHouseInstance = myApp->vikingHouse->createInstance();
    myApp->vikingHouseInstance->y.rotate(glm::radians(20.f));
}

void CleanUpApp(GraphicSampleApp* myApp)
{
    myApp->vikingHouse->deleteInstance(myApp->vikingHouseInstance);
    myApp->context->deleteModel(myApp->vikingHouse);
    netero::graphics::Application::Release();
}

#if defined(WIN_APP)
#include <WinBase.h>
#include <windows.h>
int WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
#else
int main()
{
#endif
    int              rtCode = 0;
    GraphicSampleApp myApp {};
    try {
        InitializeApp(&myApp);
        RunApplication(&myApp);
        CleanUpApp(&myApp);
    }
    catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        rtCode = 1;
    }
    catch (...) {
        std::cerr << "Fatal error." << std::endl;
        rtCode = 2;
    }
    return rtCode;
}
