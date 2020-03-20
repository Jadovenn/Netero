/**
 * Netero sources under BSD-3-Clause
 * see LICENCE.txt
 */

#include <netero/signals.hpp>

#include <Windows.h>
#include <exception>
#include <iostream>
#include <chrono>
#include <thread>

#include <netero/audio/audio.hpp>
#include <netero/audio/engine.hpp>

static netero::signals::Sinusoidal<double>	a_minor{ 5, 440, 48000, 0 };

void	callback(float* buffer, size_t size) {
	int idx = 0;
	int buffer_idx = 0;

	while (idx < size) {
		float current = static_cast<float>(a_minor(idx));
		buffer[buffer_idx] = current;
		buffer_idx += 1;
		buffer[buffer_idx] = current;
		buffer_idx += 1;
		idx += 1;
	}
}

int	main() {
	try {
		netero::audio::engine	&audio_engine = netero::audio::engine::GetInstance();
		netero::audio::RtCode	result;
		audio_engine.registerHandle(callback);
		result = audio_engine.async_start();
		if (result != netero::audio::RtCode::OK) {
			return 1;
		}

		std::chrono::time_point<std::chrono::system_clock> start = std::chrono::system_clock::now();
		netero::audio::WaveFormat format = audio_engine.getFormat();
		std::cout << "Number of channels: " << format.channels << std::endl;
		std::cout << "Frequence of the device: " << format.samplePerSecond << " Hz" << std::endl;
		while (std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now() - start).count() <= 3) {
			std::this_thread::yield();
		}
		audio_engine.async_stop();
		return 0;
	}
	catch (const std::exception & e) {
		std::cout << e.what() << std::endl;
		return 1;
	}
	return 0;
}