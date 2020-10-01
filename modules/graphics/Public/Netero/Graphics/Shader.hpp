/**
 * Netero sources under BSD-3-Clause
 * see LICENSE.txt
 */

#pragma once


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

    virtual void Load(void* someData, size_t aDataSize, ShaderStage aStage) = 0;
};

} // namespace Netero::Gfx
