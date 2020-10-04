/**
 * Netero sources under BSD-3-Clause
 * see LICENSE.txt
 */

#pragma once

#include <Netero/Graphics/Shader.hpp>
#include <netero/patterns/IFactory.hpp>

namespace Netero::Gfx {

using ShaderFactory = netero::patterns::IFactory<Shader, Renderer&>;

class DefaultFragmentShaderFactory final
    : ShaderFactory {
    public:
    ~DefaultFragmentShaderFactory() final = default;

    std::shared_ptr<Shader> Create(Renderer& aRenderer) final;
};

extern DefaultFragmentShaderFactory DefaultFragmentSF;

class DefaultVertexShaderFactory final
    : ShaderFactory {
    public:
    ~DefaultVertexShaderFactory() final = default;

    std::shared_ptr<Shader> Create(Renderer& aRenderer) final;
};

extern DefaultVertexShaderFactory DefaultVertexSF;

} // namespace Netero::Gfx
