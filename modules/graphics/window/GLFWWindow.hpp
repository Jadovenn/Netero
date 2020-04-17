/**
 * Netero sources under BSD-3-Clause
 * see LICENSE.txt
 */

#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <netero/graphics/window.hpp>

namespace netero::graphics {

    class window::impl {
    public:
        impl(unsigned width, unsigned height, const std::string& name);
        impl(const impl&) = delete;
        impl(impl&&) = delete;
        impl& operator=(const impl&) = delete;
        impl& operator=(impl&&) = delete;
        ~impl();

    	// Netero window related members
        std::string name;
        unsigned    height;
        unsigned    width;

    	// GLFW window related members
        GLFWwindow* window;
    };
	
}

