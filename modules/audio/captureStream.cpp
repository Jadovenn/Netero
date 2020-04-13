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
	if (_device) {
		_device.signals.captureStreamSig->connect(&captureSlot);
		_device.signals.deviceStreamFormatChangeSig->connect(&onFormatChangeSlot);
		_device.signals.deviceDisconnectedSig->connect(&onDisconnectedSlot);
	}
}

netero::audio::CaptureStream::CaptureStream(netero::audio::engine& engine)
	: _engine(engine)
{}

netero::audio::CaptureStream::~CaptureStream() {
	if (_device) {
		_device.signals.captureStreamSig->disconnect(&captureSlot);
		_device.signals.deviceStreamFormatChangeSig->disconnect(&onFormatChangeSlot);
		_device.signals.deviceDisconnectedSig->disconnect(&onDisconnectedSlot);
	}
}

netero::audio::engine& netero::audio::CaptureStream::getEngine() {
	return _engine;
}

