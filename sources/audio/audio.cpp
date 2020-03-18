/**
 * Netero sources under BSD-3-Clause
 * see LICENCE.txt
 */

#include <netero/audio/audio.hpp>
#include <netero/audio/engine.hpp>

netero::audio::IEngine *netero::audio::GetAudioEngine() {
	return new netero::audio::engine();
}
