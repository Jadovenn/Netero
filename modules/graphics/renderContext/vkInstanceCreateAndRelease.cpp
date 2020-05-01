/**
 * Netero sources under BSD-3-Clause
 * see LICENSE.txt
 */

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <algorithm>
#include <iostream>
#include <vector>
#include <stdexcept>
#include "renderContext/vkRenderContext.hpp"
#include "utils/vkUtils.hpp"



void netero::graphics::RenderContext::impl::releaseVulkanContext() const {
    vkDestroyDevice(this->logicalDevice, nullptr);
}

