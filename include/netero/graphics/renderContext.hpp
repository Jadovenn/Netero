/**
 * Netero sources under BSD-3-Clause
 * see LICENSE.txt
 */

#pragma once

#include <string>
#include <memory>

#if defined(NDEBUG)
#	define	IS_DEBUG	true
#else
#	define	IS_DEBUG	false
#endif

namespace netero::graphics {

	class RenderContext {
	public:
		explicit RenderContext(const std::string &appName, bool enableValidationLayers = IS_DEBUG);
		RenderContext(const RenderContext&) = delete;
		RenderContext(RenderContext&&) = delete;
		RenderContext& operator=(const RenderContext&) = delete;
		RenderContext& operator=(RenderContext&&) = delete;
		~RenderContext();

		void	initialize() const;
		void	release() const;
		
	private:
		class impl;
		std::unique_ptr<impl>	pImpl;
	};
}

