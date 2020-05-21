/**
 * Netero sources under BSD-3-Clause
 * see LICENSE.txt
 */

#pragma once

#include <array>
#include <glm/glm.hpp>
#include <vulkan/vulkan.h>

namespace netero::graphics {

    struct ColoredVertex {
        glm::vec2   pos;
        glm::vec3   color;

        static VkVertexInputBindingDescription  getBindingDescription() {
            VkVertexInputBindingDescription bindingDescription = {};

            bindingDescription.binding = 0;
            bindingDescription.stride = sizeof(ColoredVertex);
            bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
            return bindingDescription;
        }

        static std::array<VkVertexInputAttributeDescription, 2>    getAttributeDescription() {
            std::array<VkVertexInputAttributeDescription, 2>    attributeDescriptions{};
            attributeDescriptions[0].binding = 0;
            attributeDescriptions[0].location = 0;
            attributeDescriptions[0].format = VK_FORMAT_R32G32_SFLOAT;
            attributeDescriptions[0].offset = offsetof(ColoredVertex, pos);
            attributeDescriptions[1].binding = 0;
            attributeDescriptions[1].location = 1;
            attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
            attributeDescriptions[1].offset = offsetof(ColoredVertex, color);
            return attributeDescriptions;
        }
    };

    enum class ShaderStage {
        VERTEX,
        TESSELLATION,
        GEOMETRY,
        FRAGMENT,
    };

    struct Shader {
        ShaderStage         stage = ShaderStage::VERTEX;
        std::vector<char>   byteCode;
        VkShaderModule      shaderModule = nullptr;
    };

}

