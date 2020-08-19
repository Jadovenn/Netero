/**
 * Netero sources under BSD-3-Clause
 * see LICENSE.txt
 */

#pragma once

#define GLM_ENABLE_EXPERIMENTAL
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#define GLM_ENABLE_EXPERIMENTAL

#pragma warning(push)
#pragma warning(disable : 26812)
#include <vulkan/vulkan.h>
#pragma warning(pop)

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/hash.hpp>
