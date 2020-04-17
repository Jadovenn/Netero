/**
 * Netero sources under BSD-3-Clause
 * see LICENSE.txt
 */

#include <stdexcept>
#include "VulkanRenderContext.hpp"

void netero::graphics::RenderContext::impl::createVulkanContext() {
    VkInstanceCreateInfo    createInfo = {};
    uint32_t                glfwExtentionCount = 0;
	
    createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    createInfo.pApplicationInfo = &this->appInfo;
    const char** glfwExtentions = glfwGetRequiredInstanceExtensions(&glfwExtentionCount);
    createInfo.enabledExtensionCount = glfwExtentionCount;
    createInfo.ppEnabledExtensionNames = glfwExtentions;
    createInfo.enabledLayerCount = 0;
    VkResult result = vkCreateInstance(&createInfo, nullptr, &this->instance);
    if (result != VK_SUCCESS) {
        throw std::runtime_error("Failed to create instance, vkCode: " + result);
    }
}

void netero::graphics::RenderContext::impl::releaseVulkanContext() {
    vkDestroyInstance(this->instance, nullptr);
}

