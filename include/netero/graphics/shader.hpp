/**
 * Netero sources under BSD-3-Clause
 * see LICENSE.txt
 */

#pragma once

#include <vulkan/vulkan.h>

namespace netero::graphics {

enum class ShaderStage {
    VERTEX,
    TESSELLATION,
    GEOMETRY,
    FRAGMENT,
};

struct Shader {
    ShaderStage       stage = ShaderStage::VERTEX;
    std::vector<char> byteCode;
    VkShaderModule    shaderModule = nullptr;
};

} // namespace netero::graphics
