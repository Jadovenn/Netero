/**
 * Netero sources under BSD-3-Clause
 * see LICENSE.txt
 */

#include <algorithm>
#include <iostream>
#include <vector>
#include <stdexcept>
#include "VulkanRenderContext.hpp"

void netero::graphics::RenderContext::impl::createVulkanContext() {
    VkInstanceCreateInfo    createInfo = {};
    uint32_t                glfwExtentionCount = 0;

    // Setup creat info struct for createInstance call
    createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    createInfo.pApplicationInfo = &this->appInfo;
    const char** glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtentionCount);
    createInfo.enabledExtensionCount = glfwExtentionCount;
    createInfo.ppEnabledExtensionNames = glfwExtensions;
    createInfo.enabledLayerCount = 0;

   	// Check that all necessary extension are present
	uint32_t extensionCount = 0;
	vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);
	std::vector<VkExtensionProperties> extensions(extensionCount);
	vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, extensions.data());
	if (enableValidationLayers) {
		for (const auto &ext: extensions) {
			std::cout << ext.extensionName << std::endl;
		}
	}
	for (unsigned idx = 0; idx < glfwExtentionCount; ++idx) {
		auto it = std::find_if(extensions.begin(), extensions.end(), [&] (const auto ext) -> bool {
			return strcmp(glfwExtensions[idx], ext.extensionName) == 0;
		});
		if (it == extensions.end()) {
			throw std::runtime_error("Could not create instance, missing extension: " + std::string(glfwExtensions[idx]));
		}
		else if (enableValidationLayers) {
			std::cout << "Using: " << glfwExtensions[idx] << std::endl;
		}
	}

	VkResult result = vkCreateInstance(&createInfo, nullptr, &this->instance);
    if (result != VK_SUCCESS) {
        throw std::runtime_error("Failed to create instance, vkCode: " + std::to_string(result));
    }
}

void netero::graphics::RenderContext::impl::releaseVulkanContext() {
    vkDestroyInstance(this->instance, nullptr);
}

