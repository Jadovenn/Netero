/**
 * Netero sources under BSD-3-Clause
 * see LICENSE.txt
 */

#pragma once

#include <cstddef>
#include <memory>

#include <Netero/Graphics/Renderer.hpp>

namespace Netero::Gfx {

enum class ShaderStage {
    VERTEX = 0,
    TESSELLATION = 1,
    GEOMETRY = 2,
    FRAGMENT = 3,
};

class Shader {
    public:
    virtual ~Shader() = default;

    static std::unique_ptr<Shader> New(Renderer&, const unsigned*, size_t, ShaderStage);

    [[nodiscard]] virtual ShaderStage GetShaderStage() = 0;

    protected:
    Shader() = default;
};

class ShaderEnvironment {
    public:
    virtual void SetShader(std::shared_ptr<Shader>) = 0;
};

} // namespace Netero::Gfx
