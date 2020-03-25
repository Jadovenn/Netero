/**
 * Netero sources under BSD-3-Clause
 * see LICENCE.txt
 */

#pragma once

#include <netero/audio/engine.hpp>
#include <netero/audio/mixer.hpp>

/**
 * @file device.hpp
 * Audio rendering device declaration
 */

namespace netero::audio {

	/**
	 * @class device
	 * The device is a mixer that automaticaly connect to the audio engine.
	 * It is the root mixer.
	 */
	class device: public mixer {
	private:
		device();
	public:
		/**
		 * @methode GetAudioDevice
		 * @brief return the current audio device
		 */
		static device& GetAudioDevice();
		~device();

		void		handle(float* buffer, size_t size);
		WaveFormat&	getWaveFormat();

	private:
		netero::audio::engine& _engine;
	};

}

