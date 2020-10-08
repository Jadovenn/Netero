/**
 * Netero sources under BSD-3-Clause
 * see LICENSE.txt
 */

#include "Renderer/Drawable/Geometry.hpp"

#include <memory>

#include "Renderer/Renderer.hpp"
#include "Renderer/Shader/Shader.hpp"

namespace Netero::Gfx {

GeometryImpl::GeometryImpl(Context &aContext): myContext(aContext)
{
}

GeometryImpl::~GeometryImpl()
{
    Release();
    Teardown();
}

std::shared_ptr<Geometry> Geometry::New(Renderer &aRenderer)
{
    if (auto *renderer = dynamic_cast<RendererImpl *>(&aRenderer)) {
        return std::make_unique<GeometryImpl>(renderer->GetContext());
    }
    return nullptr;
}

void GeometryImpl::SetVertices(const Vertices &someVertices)
{
    myVertices = someVertices;
    for (unsigned index = 0; index < myVertices.size(); ++index) {
        myIndices.push_back(index);
    }
}

void GeometryImpl::SetVerticesWithIndices(const Vertices &someVertices, const Indices &someIndices)
{
    myVertices = someVertices;
    myIndices = someIndices;
}

void GeometryImpl::SetShader(std::shared_ptr<Shader> aShader)
{
    if (auto *shader = dynamic_cast<ShaderImpl *>(aShader.get())) {
        if (shader->IsValid()) {
            myShaders[static_cast<int>(aShader->GetShaderStage())] = aShader;
        }
        // may be assert again here with a different err msg
    }
    // can assert here in else statement
}

} // namespace Netero::Gfx
