/**
 * Netero sources under BSD-3-Clause
 * see LICENSE.txt
 */

#pragma once

#include "Vulkan/Vulkan.hpp"

namespace Netero::Gfx {

struct Descriptor {
    VkDescriptorType   myDescriptorType = VK_DESCRIPTOR_TYPE_MAX_ENUM;
    VkShaderStageFlags myShaderStage = VK_SHADER_STAGE_FLAG_BITS_MAX_ENUM;
    uint32_t           myBinding = 0;
};

} // namespace Netero::Gfx
