/**
 * Netero sources under BSD-3-Clause
 * see LICENSE.txt
 */

#include "Renderer/Shader/DefaultShaders.hpp"

#include "Renderer/Shader/Default/DefaultFragment.h"
#include "Renderer/Shader/Default/DefaultVertex.h"

namespace Netero::Gfx {

DefaultFragmentShaderFactory DefaultFragmentSF;

std::shared_ptr<Shader> DefaultFragmentShaderFactory::Create(Renderer &aRenderer)
{
    return Shader::New(aRenderer,
                       DefaultFragmentData,
                       DefaultFragmentWordCount * sizeof(unsigned),
                       ShaderStage::FRAGMENT);
}

DefaultVertexShaderFactory DefaultVertexSF;

std::shared_ptr<Shader> DefaultVertexShaderFactory::Create(Renderer &aRenderer)
{
    return Shader::New(aRenderer,
                       DefaultVertexData,
                       DefaultVertexWordCount * sizeof(unsigned),
                       ShaderStage::FRAGMENT);
}

} // namespace Netero::Gfx
