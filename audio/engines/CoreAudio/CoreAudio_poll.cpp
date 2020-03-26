/**
 * Netero sources under BSD-3-Clause
 * see LICENCE.txt
 */

#include "CoreAudio.hpp"

netero::audio::RtCode netero::audio::engine::impl::start() {
	return RtCode::OK;
}

netero::audio::RtCode netero::audio::engine::impl::stop() {
	return RtCode::OK;
}

netero::audio::RtCode netero::audio::engine::impl::poll() {
	return RtCode::OK;
}
