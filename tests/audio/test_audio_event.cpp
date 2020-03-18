/**
 * Netero sources under BSD-3-Clause
 * see LICENCE.txt
 */

#include <netero/signals.hpp>
#include <netero/audio/audio.hpp>
#include <Windows.h>

static netero::signals::Sinusoidal<double>	a_minor { 1, 440, 48000, 0 };

void	callback(float* buffer, size_t size) {
	int idx = 0;
	int buffer_idx = 0;

	Sleep(10);
	while (idx < size) {
		float current = a_minor(idx);
		buffer[buffer_idx] = current;
		buffer_idx += 1;
		buffer[buffer_idx] = current;
		buffer_idx += 1;
		idx += 1;
	}
}

int	main() {
	netero::audio::IEngine* audio_engine = netero::audio::GetAudioEngine();
	if (!audio_engine) {
		return 1;
	}
	audio_engine->registerCB(callback);
	if (audio_engine->start() != netero::audio::OK) {
		delete audio_engine;
		return 1;
	}
	while (1);
	audio_engine->stop();
	delete audio_engine;
	return 0;
}

