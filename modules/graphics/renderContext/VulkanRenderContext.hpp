/**
 * Netero sources under BSD-3-Clause
 * see LICENSE.txt
 */

#pragma once

#include <vulkan/vulkan.h>
#include <GLFW/glfw3.h>

#include <netero/graphics/renderContext.hpp>

namespace netero::graphics {
	class RenderContext::impl {
	public:
		impl(const std::string& appName);
		impl(const impl&) = delete;
		impl(impl&&) = delete;
		impl& operator=(const impl&) = delete;
		impl& operator=(impl&&) = delete;
		~impl();

		void	createVulkanContext();
		void	releaseVulkanContext();

		const std::string	appName;
		
		VkInstance			instance;
		VkApplicationInfo	appInfo;
	};
}

