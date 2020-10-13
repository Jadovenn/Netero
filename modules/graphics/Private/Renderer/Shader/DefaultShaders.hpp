/**
 * Netero sources under BSD-3-Clause
 * see LICENSE.txt
 */

#pragma once

#include <Netero/Graphics/Shader.hpp>
#include <netero/patterns/IFactory.hpp>

namespace Netero::Gfx {

using ShaderFactory = netero::patterns::IFactory<Shader, Renderer&>;

class GeometryFragmentShaderFactory final
    : ShaderFactory {
    public:
    ~GeometryFragmentShaderFactory() final = default;

    std::shared_ptr<Shader> Create(Renderer& aRenderer) final;
};

extern GeometryFragmentShaderFactory DefaultFragmentSF;

class GeometryVertexShaderFactory final
    : ShaderFactory {
    public:
    ~GeometryVertexShaderFactory() final = default;

    std::shared_ptr<Shader> Create(Renderer& aRenderer) final;
};

extern GeometryVertexShaderFactory DefaultVertexSF;

} // namespace Netero::Gfx
