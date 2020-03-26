/**
 * Netero sources under BSD-3-Clause
 * see LICENCE.txt
 */

#define _USE_MATH_DEFINES
#include <cmath>
#include <type_traits>
#include <exception>
#include <iostream>
#include <chrono>

#include <netero/audio/engine.hpp>

class Sinusoidal {
public:
	Sinusoidal(double amplitude, double pulsation, double freq, double phase)
		: amplitude(amplitude),
		pulsation((2 * M_PI * pulsation) / freq),
		phase(phase)
	{}

	double	operator()(double t) {
		return amplitude * sin(pulsation * t + phase);
	}

	double amplitude;
	double pulsation;
	double phase;
};

static Sinusoidal	a_minor { 1, 440, 48000, 0 };

void	callback(float* buffer, size_t size) {
	int idx = 0;

	while (idx < size * 2) {
		float current = static_cast<float>(a_minor(idx));
		buffer[idx] = current;
		buffer[idx + 1] = current;
		idx += 2;
	}
}

int	main() {
	try {
		netero::audio::engine	&audio_engine = netero::audio::engine::GetInstance();
		audio_engine.registerHandle(callback);
		if (audio_engine.start() != netero::audio::RtCode::OK) {
			return 1;
		}
		netero::audio::WaveFormat format = audio_engine.getFormat();
		std::cout << "Number of channels: " << format.channels << std::endl;
		std::cout << "Frequence of the device: " << format.samplingFrequency << " Hz" << std::endl;
		std::chrono::time_point<std::chrono::system_clock> start = std::chrono::system_clock::now();
		while (std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now() - start).count() <= 10) {
			audio_engine.poll();
		}
		audio_engine.stop();
	}
	catch (const std::exception & e) {
		std::cout << e.what() << std::endl;
		return 1;
	}
	catch (...) {
		std::cout << "Unknown exception" << std::endl;
		return 2;
	}
	return 0;
}

