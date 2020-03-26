/**
 * Netero sources under BSD-3-Clause
 * see LICENCE.txt
 */

#include "CoreAudio.hpp"

void netero::audio::engine::impl::CORE_AUDIO_init() {

}

void netero::audio::engine::impl::CORE_AUDIO_release() {

}

netero::audio::WaveFormat netero::audio::engine::impl::getFormat() {
	return WaveFormat{};
}

size_t netero::audio::engine::impl::getBufferSize() {
	return 0;
}
