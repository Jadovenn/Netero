/**
 * Netero sources under BSD-3-Clause
 * see LICENSE.txt
 */

#pragma once

#include "Vulkan/Vulkan.hpp"

#include <Netero/Graphics/Attributes/Shader.hpp>

namespace Netero::Gfx {

class Context;

class ShaderImpl final: public Shader {
    public:
    ShaderImpl(Context& aContext, const unsigned* someData, size_t aSize, ShaderStage aStage);

    [[nodiscard]] ShaderStage GetShaderStage() override { return myShaderStage; }

    [[nodiscard]] bool IsValid() const { return myIsValid; }

    private:
    Context&       myContext;
    bool           myIsValid;
    VkShaderModule myShaderModule;

    ShaderStage myShaderStage;
};

} // namespace Netero::Gfx
