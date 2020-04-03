/**
 * Netero sources under BSD-3-Clause
 * see LICENCE.txt
 */

#include <netero/audio/engine.hpp>

netero::audio::engine::engine(const InitStrategy strategy)
    : _backend(netero::audio::backend::GetInstance())
{
    _backend.setCaptureCallback(std::bind(&netero::audio::engine::captureHandler,
        this,
        std::placeholders::_1,
        std::placeholders::_2));
    _backend.setCaptureErrorCallback(std::bind(&netero::audio::engine::captureErrorHandler,
        this,
        std::placeholders::_1));
    _backend.setRenderErrorCallback(std::bind(&netero::audio::engine::renderErrorHandler,
        this,
        std::placeholders::_1));
}

netero::audio::engine::~engine() {
    _backend.stopRender();
    _backend.stopCapture();
}

const std::vector<netero::audio::device> &netero::audio::engine::getRenderDevices() {
    return _backend.getRenderDevices();
}

const std::vector<netero::audio::device> &netero::audio::engine::getCaptureDevices() {
    return _backend.getCaptureDevices();
}

netero::audio::StreamFormat   netero::audio::engine::getRenderFormat() {
    return _backend.getRenderFormat();
}

netero::audio::StreamFormat   netero::audio::engine::getCaptureFormat() {
    return _backend.getCaptureFormat();
}

netero::audio::RtCode   netero::audio::engine::setRenderDevice(const netero::audio::device& device) {
    RtCode code = _backend.setRenderDevice(device);
    if (code == RtCode::OK) {
        renderFormatChangeSig(_backend.getRenderFormat());
    }
    return code;
}

netero::audio::RtCode   netero::audio::engine::setCaptureDevice(const netero::audio::device& device) {
    RtCode code = _backend.setCaptureDevice(device);
    if (code == RtCode::OK) {
        captureFormatChangeSig(_backend.getCaptureFormat());
    }
    return code;
}

netero::audio::RtCode   netero::audio::engine::setRenderCallback(const netero::audio::RenderCallback &callback) {
    return _backend.setRenderCallback(callback);
}

netero::audio::RtCode   netero::audio::engine::startRender() {
    return _backend.startRender();
}

netero::audio::RtCode   netero::audio::engine::stopRender() {
    return _backend.stopRender();
}

void    netero::audio::engine::captureHandler(const float* buffer, const size_t frames) {
    captureStreamSig(buffer, frames);
}

void    netero::audio::engine::renderErrorHandler(const std::string& error) {
    renderErrorSig(error);
}

void    netero::audio::engine::captureErrorHandler(const std::string& error) {
    captureErrorSig(error);
}

netero::audio::RtCode   netero::audio::engine::startCapture() {

    return _backend.startCapture();
}

netero::audio::RtCode   netero::audio::engine::stopCapture() {
    return _backend.stopCapture();
}

