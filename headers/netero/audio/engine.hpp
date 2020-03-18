/**
 * Netero sources under BSD-3-Clause
 * see LICENCE.txt
 */

#pragma once

#include <memory>

#include <netero/audio/audio.hpp>

namespace netero::audio {

	class engine: public netero::audio::IEngine {
	private:
		class impl;
		std::unique_ptr<impl>	pImpl;
	public:

		engine();
		void	registerCB(std::function<void(float*, size_t)> cb) final;
		RtCode	stop() final;
		RtCode	start() final;
	};
}