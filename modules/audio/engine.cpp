/**
 * Netero sources under BSD-3-Clause
 * see LICENCE.txt
 */

#include <netero/audio/engine.hpp>

netero::audio::engine::engine() noexcept
    : _backend(netero::audio::backend::GetInstance())
{
        this->_backend.setDeviceDisconnectedCallback([&](const netero::audio::device &device) -> void {
            this->deviceDisconnectedSig(device);
        });
}

netero::audio::engine& netero::audio::engine::getInstance() noexcept {
    static std::unique_ptr<netero::audio::engine>   audioEngine(new netero::audio::engine);
    return *audioEngine;
}

std::vector<netero::audio::device>	netero::audio::engine::getRenderDevices() const {
    return _backend.getRenderDevices();
}

const netero::audio::device& netero::audio::engine::getDefaultRenderDevice() const {
    return _backend.getDefaultRenderDevice();
}

std::vector<netero::audio::device>	netero::audio::engine::getCaptureDevices() const {
    return _backend.getCaptureDevices();
}

const netero::audio::device& netero::audio::engine::getDefaultCaptureDevice() const {
    return _backend.getDefaultCaptureDevice();
}

netero::audio::device::events& netero::audio::engine::getDeviceEvents(const netero::audio::device &device) {
    return _backend.getDeviceEvents(device);
}

netero::audio::RtCode       netero::audio::engine::deviceStartRendering(const netero::audio::device& device) {
    return _backend.deviceStartRendering(device);
}

netero::audio::RtCode       netero::audio::engine::deviceStopRendering(const netero::audio::device& device) {
    return _backend.deviceStopRendering(device);
}

netero::audio::RtCode       netero::audio::engine::deviceStartRecording(const netero::audio::device& device) {
    return _backend.deviceStartRecording(device);
}

netero::audio::RtCode       netero::audio::engine::deviceStopRecording(const netero::audio::device& device) {
    return _backend.deviceStopRecording(device);
}

