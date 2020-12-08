/**
 * Netero sources under BSD-3-Clause
 * see LICENSE.txt
 */

#pragma once

#include <cstddef>
#include <memory>

#include <Netero/Graphics/Renderer.hpp>
#include <Netero/Graphics/Attributes/Attribute.hpp>

namespace Netero::Gfx {

enum class ShaderStage {
    VERTEX = 0,
    TESSELLATION = 1,
    GEOMETRY = 2,
    FRAGMENT = 3,
};

class Shader: public Attribute {
    public:
    virtual ~Shader() = default;

    [[nodiscard]] virtual ShaderStage GetShaderStage() = 0;

    protected:
    Shader() = default;
};

class VertexShader: public Shader {
    public:
    static std::shared_ptr<Shader> New(Renderer*, const unsigned*, size_t);

    private:
    VertexShader() = default;
};

class TessellationShader: public Shader {
    public:
    static std::shared_ptr<Shader> New(Renderer*, const unsigned*, size_t);

    private:
    TessellationShader() = default;
};

class GeometryShader: public Shader {
    public:
    static std::shared_ptr<Shader> New(Renderer*, const unsigned*, size_t);

    private:
    GeometryShader() = default;
};

class FragmentShader: public Shader {
    public:
    static std::shared_ptr<Shader> New(Renderer*, const unsigned*, size_t);

    private:
    FragmentShader() = default;
};

} // namespace Netero::Gfx
