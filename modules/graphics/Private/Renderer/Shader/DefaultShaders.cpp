/**
 * Netero sources under BSD-3-Clause
 * see LICENSE.txt
 */

#include "Renderer/Shader/DefaultShaders.hpp"

#include "Renderer/Shader/Default/GeometryVertex.h"
#include "Renderer/Shader/Default/GeometryFragment.h"

namespace Netero::Gfx {

GeometryFragmentShaderFactory DefaultFragmentSF;

std::shared_ptr<Shader> GeometryFragmentShaderFactory::Create(Renderer &aRenderer)
{
    return Shader::New(aRenderer,
                       GeometryFragmentData,
                       GeometryFragmentWordCount * sizeof(unsigned),
                       ShaderStage::FRAGMENT);
}

GeometryVertexShaderFactory DefaultVertexSF;

std::shared_ptr<Shader> GeometryVertexShaderFactory::Create(Renderer &aRenderer)
{
    return Shader::New(aRenderer,
                       GeometryVertexData,
                       GeometryVertexWordCount * sizeof(unsigned),
                       ShaderStage::FRAGMENT);
}

} // namespace Netero::Gfx
