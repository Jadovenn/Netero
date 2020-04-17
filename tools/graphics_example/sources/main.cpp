/**
 * Netero sources under BSD-3-Clause
 * see LICENSE.txt
 */

#include <iostream>
#include <netero/graphics/window.hpp>
#include <netero/graphics/renderContext.hpp>

int	main() {
	try {
		const netero::graphics::window	m_window(800, 600, "GraphicsExample");
		const netero::graphics::RenderContext m_context("GraphicsExample");
		m_window.initialize();
		m_context.initialize();
		m_window.runLoop();
		m_context.release();
	}
	catch (const std::exception &e) {
		std::cerr << e.what() << std::endl;
		return 1;
	}
	catch(...) {
		std::cerr << "Fatal error." << std::endl;
		return 1;
	}
	return 0;
}

