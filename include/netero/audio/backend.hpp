/**
 * Netero sources under BSD-3-Clause
 * see LICENCE.txt
 */

#pragma once

#include <memory>
#include <functional>

#include <netero/audio/audio.hpp>

namespace netero::audio {

	class backend {
	private:
		class impl;
		std::unique_ptr<impl>	pImpl;
		backend();
	public:

		static backend& GetInstance();
		~backend();
		void	registerHandle(const std::function<void(float*, size_t)> &cb);
		WaveFormat	getFormat();
		RtCode	stop();
		RtCode	start();
		RtCode	poll();

		size_t	getBufferSize();

	};
}