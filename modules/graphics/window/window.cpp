/**
 * Netero sources under BSD-3-Clause
 * see LICENSE.txt
 */

#include "GLFWWindow.hpp"

netero::graphics::window::window(unsigned width, unsigned height, const std::string& name)
    :   _pImpl(std::make_unique<window::impl>(width, height, name))
{}

netero::graphics::window::~window() {
    glfwDestroyWindow(_pImpl->window);
    glfwTerminate();
}

netero::graphics::window::impl::impl(unsigned width, unsigned height, const std::string& name)
    :   name(name),
		height(height),
		width(width)
{}

netero::graphics::window::impl::~impl() = default;

void netero::graphics::window::initialize() const {
    glfwInit();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
    _pImpl->window = glfwCreateWindow(_pImpl->width,
        _pImpl->height,
        _pImpl->name.c_str(),
        nullptr,
        nullptr);
}


void netero::graphics::window::runLoop() const {
	while (!glfwWindowShouldClose(_pImpl->window)) {
        glfwPollEvents();
	}
}
