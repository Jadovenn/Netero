/**
 * Netero sources under BSD-3-Clause
 * see LICENSE.txt
 */

#include <netero/audio/deviceManager.hpp>
#include <netero/audio/engine.hpp>

netero::audio::engine& netero::audio::engine::getInstance() {
	static std::unique_ptr<netero::audio::engine> audioEngine(new audio::engine);
	return *audioEngine;
}

netero::audio::engine::engine() {
	// Start the rendering as soon as the engine is ready
	auto& deviceManager = DeviceManager::getInstance();
	this->_renderDevice = deviceManager.getDefaultRenderDevice();
	this->_mixer.setFormat(this->_renderDevice.format);
	this->_renderDevice.signals.renderStreamSig->connect(&this->_mixer.renderSlot);
	deviceManager.deviceStartRendering(this->_renderDevice);
	// But only start the capture if an entity register for it.
	this->_captureDevice = deviceManager.getDefaultCaptureDevice();
}

netero::audio::RtCode netero::audio::engine::setRenderDevice(const netero::audio::device& device) {
	auto& deviceManager = DeviceManager::getInstance();
	RtCode	result;

	if (!device) { return RtCode::ERR_NO_SUCH_DEVICE; }
	if (this->_renderDevice) {
		this->_mixer.pause();
		this->_renderDevice.signals.renderStreamSig->disconnect(&this->_mixer.renderSlot);
		result = deviceManager.deviceStopRendering(this->_renderDevice);
		if (result != RtCode::OK) { return result; }
	}
	this->_renderDevice = device;
	result = deviceManager.deviceStartRendering(this->_renderDevice);
	if (result != RtCode::OK && result != RtCode::ERR_ALREADY_RUNNING) { return result; }
	this->_mixer.setFormat(this->_renderDevice.format);
	this->_renderDevice.signals.renderStreamSig->connect(&this->_mixer.renderSlot);
	this->_mixer.play();
	return RtCode::OK;
}

netero::audio::RtCode netero::audio::engine::setCaptureDevice(const netero::audio::device& device) {
	auto& deviceManager = DeviceManager::getInstance();
	RtCode	result;

	if (!device) { return RtCode::ERR_NO_SUCH_DEVICE; }
	if (this->_captureDevice) {
		if (this->_captureEntity) {
			this->_captureEntity->pause();
			this->_captureDevice.signals.captureStreamSig->disconnect(&this->_captureEntity->captureSlot);
		}
		result = deviceManager.deviceStopRecording(this->_captureDevice);
		if (result != RtCode::OK && result != RtCode::ERR_DEVICE_NOT_RUNNING) { return result; }
	}
	this->_captureDevice = device;
	if (this->_captureEntity) {
		this->_captureDevice.signals.captureStreamSig->connect(&this->_captureEntity->captureSlot);
		result = deviceManager.deviceStartRecording(this->_captureDevice);
		if (result != RtCode::OK && result != RtCode::ERR_ALREADY_RUNNING) { return result; }
		this->_captureEntity->record();
	}
	return RtCode::OK;
}

netero::audio::RtCode netero::audio::engine::disconnectRenderDevice() {
	auto& deviceManager = DeviceManager::getInstance();

	if (this->_renderDevice) {
		this->_mixer.pause();
		this->_renderDevice.signals.renderStreamSig->disconnect(&this->_mixer.renderSlot);
		const RtCode result = deviceManager.deviceStopRendering(this->_renderDevice);
		if (result != RtCode::OK) { return result; }
	}
	else {
		return RtCode::ERR_DEVICE_NOT_RUNNING;
	}
	return RtCode::OK;
}

netero::audio::RtCode netero::audio::engine::disconnectCaptureDevice() {
	auto& deviceManager = DeviceManager::getInstance();

	if (this->_captureDevice) {
		if (this->_captureEntity) {
			this->_captureEntity->pause();
			this->_captureDevice.signals.captureStreamSig->disconnect(&this->_captureEntity->captureSlot);
		}
		const RtCode result = deviceManager.deviceStopRecording(this->_captureDevice);
		if (result != RtCode::OK) { return result; }
	}
	else {
		return RtCode::ERR_DEVICE_NOT_RUNNING;
	}
	return RtCode::OK;
}
