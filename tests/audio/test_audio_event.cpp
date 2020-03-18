/**
 * Netero sources under BSD-3-Clause
 * see LICENCE.txt
 */

#include <netero/signals.hpp>
#include <netero/audio/audio.hpp>

static netero::signals::Sinusoidal	e_minor { 1, 440, 48000, 0 };

void	callback(float* buffer, size_t size) {
	// fill buffer here
}

int	main() {
	netero::audio::IEngine* audio_engine = netero::audio::GetAudioEngine();
	if (!audio_engine) {
		return 1;
	}
	audio_engine->registerCB(callback);
	audio_engine->start();
	// Sleep(3000);
	audio_engine->stop();
	delete audio_engine;
	return 0;
}

