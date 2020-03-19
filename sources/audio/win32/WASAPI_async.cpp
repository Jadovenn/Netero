/**
 * Netero sources under BSD-3-Clause
 * see LICENCE.txt
 */

#include <memory>
#include "WASAPI.hpp"

netero::audio::RtCode   netero::audio::engine::impl::async_start() {
    if (!_cb) {
        return RtCode::ERR_MISSING_CALLBACK;
    }
    _state.store(state::RUNNING, std::memory_order_release);
    _thread = std::make_unique<std::thread>(std::bind(&netero::audio::engine::impl::handle, this));
    return RtCode::OK;
}

netero::audio::RtCode   netero::audio::engine::impl::async_stop() {
    _state.store(state::STOP, std::memory_order_release);
    _thread->join();
    _thread.reset();
    _state.store(state::OFF, std::memory_order_release);
    return RtCode::OK;
}

void    netero::audio::engine::impl::handle() {
    start();
    while (_state.load(std::memory_order_acquire) == state::RUNNING) {
        poll();
    }
    stop();
}

