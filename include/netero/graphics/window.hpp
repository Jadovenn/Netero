/**
 * Netero sources under BSD-3-Clause
 * see LICENSE.txt
 */

#pragma once

#include <string>
#include <memory>

namespace netero::graphics {

	enum class WindowMode {
		FIX,
		RESIZABLE,
		FULLSCREEN,
		WINDOWED,
	};

	class window {
	public:
		window(unsigned width, unsigned height, const std::string& name);
		~window();

		void	initialize() const;

		void	runLoop() const;
		
	private:
		class impl;
		std::unique_ptr<impl>	_pImpl;
	};
}

