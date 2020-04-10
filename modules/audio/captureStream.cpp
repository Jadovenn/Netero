/**
 * Netero sources under BSD-3-Clause
 * see LICENCE.txt
 */

#include <netero/audio/captureStream.hpp>

netero::audio::CaptureStream::CaptureStream(netero::audio::engine& engine,
	const netero::audio::device& device)
	: _engine(engine),
	_device(device)
{
	try {
		auto& deviceSignals = _engine.getDeviceEvents(_device);
		deviceSignals.captureStreamSig.connect(&captureSlot);
		deviceSignals.deviceStreamFormatChangeSig.connect(&onFormatChangeSlot);
		deviceSignals.deviceDisconnectedSig.connect(&onDisconnectedSlot);
	}
	catch (...) {}
}

netero::audio::CaptureStream::CaptureStream(netero::audio::engine& engine)
	: _engine(engine)
{}

netero::audio::CaptureStream::~CaptureStream() {
	try {
		auto& deviceSignals = _engine.getDeviceEvents(_device);
		deviceSignals.captureStreamSig.disconnect(&captureSlot);
		deviceSignals.deviceStreamFormatChangeSig.disconnect(&onFormatChangeSlot);
		deviceSignals.deviceDisconnectedSig.disconnect(&onDisconnectedSlot);
	}
	catch (...) {}
}

netero::audio::engine& netero::audio::CaptureStream::getEngine() {
	return _engine;
}

