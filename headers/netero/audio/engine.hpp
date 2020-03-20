/**
 * Netero sources under BSD-3-Clause
 * see LICENCE.txt
 */

#pragma once

#include <memory>

#include <netero/audio/audio.hpp>

namespace netero::audio {

	class engine {
	private:
		class impl;
		std::unique_ptr<impl>	pImpl;
		engine();
	public:

		static engine& GetInstance();
		~engine();
		void	registerHandle(std::function<void(float*, size_t)> cb);
		WaveFormat	getFormat();
		RtCode	stop();
		RtCode	start();
		RtCode	async_start();
		RtCode	async_stop();
		RtCode	poll();
	};
}