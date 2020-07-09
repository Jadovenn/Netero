/**
 * Netero sources under BSD-3-Clause
 * see LICENSE.txt
 */

#include <netero/audio/deviceManager.hpp>

netero::audio::DeviceManager::DeviceManager() noexcept
    : _backend(netero::audio::backend::GetInstance())
{
        this->_backend.setDeviceDisconnectedCallback([&](const netero::audio::device &device) -> void {
            this->deviceDisconnectedSig(device);
        });
}

netero::audio::DeviceManager& netero::audio::DeviceManager::getInstance() noexcept {
    static std::unique_ptr<netero::audio::DeviceManager>   audioEngine(new netero::audio::DeviceManager);
    return *audioEngine;
}

std::vector<netero::audio::device>	netero::audio::DeviceManager::getRenderDevices() const {
    return _backend.getRenderDevices();
}

const netero::audio::device& netero::audio::DeviceManager::getDefaultRenderDevice() const {
    return _backend.getDefaultRenderDevice();
}

std::vector<netero::audio::device>	netero::audio::DeviceManager::getCaptureDevices() const {
    return _backend.getCaptureDevices();
}

const netero::audio::device& netero::audio::DeviceManager::getDefaultCaptureDevice() const {
    return _backend.getDefaultCaptureDevice();
}

netero::audio::RtCode       netero::audio::DeviceManager::deviceStartRendering(const netero::audio::device& device) {
    return _backend.deviceStartRendering(device);
}

netero::audio::RtCode       netero::audio::DeviceManager::deviceStopRendering(const netero::audio::device& device) {
    return _backend.deviceStopRendering(device);
}

netero::audio::RtCode       netero::audio::DeviceManager::deviceStartRecording(const netero::audio::device& device) {
    return _backend.deviceStartRecording(device);
}

netero::audio::RtCode       netero::audio::DeviceManager::deviceStopRecording(const netero::audio::device& device) {
    return _backend.deviceStopRecording(device);
}

