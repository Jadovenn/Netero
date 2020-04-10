/**
 * Netero sources under BSD-3-Clause
 * see LICENCE.txt
 */

#include <netero/audio/engine.hpp>

netero::audio::engine::engine(const InitStrategy strategy)
    : _backend(netero::audio::backend::GetInstance())
{}

netero::audio::engine::~engine() {
}

const std::vector<netero::audio::device>	netero::audio::engine::getRenderDevices() {
    return _backend.getRenderDevices();
}

const netero::audio::device& netero::audio::engine::getDefaultRenderDevice() {
    return _backend.getDefaultRenderDevice();
}

const std::vector<netero::audio::device>	netero::audio::engine::getCaptureDevices() {
    return _backend.getCaptureDevices();
}

const netero::audio::device& netero::audio::engine::getDefaultCaptureDevice() {
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

