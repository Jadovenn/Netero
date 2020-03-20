/**
 * Netero sources under BSD-3-Clause
 * see LICENCE.txt
 */

#pragma once

#include <netero/audio/engine.hpp>
#include <netero/audio/mixer.hpp>

namespace netero::audio {

	class device: public mixer {
	private:
		device();
	public:
		~device();

		static device& GetAudioDevice();
		void	handle(float* buffer, size_t size);

	private:
		netero::audio::engine& _engine;
	};

}

