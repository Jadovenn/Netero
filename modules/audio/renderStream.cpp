/**
 * Netero sources under BSD-3-Clause
 * see LICENCE.txt
 */

#include <netero/audio/renderStream.hpp>

netero::audio::RenderStream::RenderStream(netero::audio::engine& engine,
	const netero::audio::device& device)
	:	_engine(engine),
		_device(device)
{
	if (_device) {
		_device.signals.renderStreamSig->connect(&renderSlot);
		_device.signals.deviceStreamFormatChangeSig->connect(&onFormatChangeSlot);
	}
}

netero::audio::RenderStream::RenderStream(netero::audio::engine& engine)
	: _engine(engine)
{}

netero::audio::RenderStream::~RenderStream() {
	if (_device) {
		_device.signals.renderStreamSig->disconnect(&renderSlot);
		_device.signals.deviceStreamFormatChangeSig->disconnect(&onFormatChangeSlot);
	}
}

netero::audio::engine& netero::audio::RenderStream::getEngine() {
	return _engine;
}

