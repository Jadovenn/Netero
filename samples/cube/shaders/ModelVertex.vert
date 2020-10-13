/**
 * Netero sources under BSD-3-Clause
 * see LICENSE.txt
 */

#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(binding = 0) uniform uniformBufferObject {
    mat4 view;
    mat4 proj;
} ubo;

// INPUT_RATE_VERTEX
layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inColor;
layout(location = 2) in vec3 inTexCord;

// INPUT_RATE_INSTANCE
layout(location = 3) in mat4 inModel;

layout(location = 0) out vec3 fragColor;

void main() {
    gl_Position = ubo.proj * ubo.view * inModel * vec4(inPosition, 1.0);
    fragColor = inColor;
}

