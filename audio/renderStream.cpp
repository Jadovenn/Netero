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
	try {
		auto& deviceSignals = _engine.getDeviceEvents(_device);
		deviceSignals.renderStreamSig.connect(&renderSlot);
		deviceSignals.deviceStreamFormatChangeSig.connect(&onFormatChangeSlot);
	}
	catch (...) {}
}

netero::audio::RenderStream::RenderStream(netero::audio::engine& engine)
	: _engine(engine)
{}

netero::audio::RenderStream::~RenderStream() {
	try {
		auto& deviceSignals = _engine.getDeviceEvents(_device);
		deviceSignals.captureStreamSig.disconnect(&renderSlot);
		deviceSignals.deviceStreamFormatChangeSig.disconnect(&onFormatChangeSlot);
	}
	catch (...) {}
}

netero::audio::engine& netero::audio::RenderStream::getEngine() {
	return _engine;
}

