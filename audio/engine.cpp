/**
 * Netero sources under BSD-3-Clause
 * see LICENCE.txt
 */

#include <netero/audio/engine.hpp>

netero::audio::engine::engine(const InitStrategy strategy)
    : _backend(netero::audio::backend::GetInstance())
{}

netero::audio::engine::~engine() {
    _backend.stopRender();
}

const std::vector<netero::audio::device> &netero::audio::engine::getOutDevices() {
    return _backend.getOutputDevices();
}

const std::vector<netero::audio::device> &netero::audio::engine::getInDevices() {
    return _backend.getInputDevices();
}

netero::audio::WaveFormat   netero::audio::engine::getOutputFormat() {
    return _backend.getOutputFormat();
}

netero::audio::RtCode   netero::audio::engine::setOutputDevice(const netero::audio::device& device) {
    RtCode code = _backend.setOutputDevice(device);
    if (code == RtCode::OK) {
        formatChangeSig(_backend.getOutputFormat());
    }
    return code;
}

netero::audio::RtCode   netero::audio::engine::setRenderCallback(const netero::audio::backend::RenderCallback &callback) {
    return _backend.setRenderCallback(callback);
}

netero::audio::RtCode   netero::audio::engine::startRender() {
    return _backend.startRender();
}

netero::audio::RtCode   netero::audio::engine::stopRender() {
    return _backend.stopRender();
}

