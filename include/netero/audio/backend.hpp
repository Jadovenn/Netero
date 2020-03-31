/**
 * Netero sources under BSD-3-Clause
 * see LICENCE.txt
 */

#pragma once

#include <memory>
#include <functional>

#include <netero/audio/audio.hpp>

namespace netero::audio {

	class backend {
	public:
		using RenderCallback = std::function<void(float*, size_t)>;
		using CaptureCallback = std::function<void(const float*, const size_t)>;
	private:
		class impl;
		std::unique_ptr<impl>	pImpl;
		backend();
	public:
		static backend& GetInstance();
		~backend();

		const std::vector<device>&	getOutputDevices();
		RtCode						setOutputDevice(const device&);
		WaveFormat					getOutputFormat();
		RtCode						startRender();
		RtCode						stopRender();
		RtCode						setRenderCallback(const RenderCallback&);

		const std::vector<device>&	getInputDevices();
		RtCode						setInputDevice(const device&);
		WaveFormat					getInputFormat();
		RtCode						capturingThreadHandle();
		RtCode						startCapture();
		RtCode						stopCapture();
		RtCode						setCaptureCallback(const CaptureCallback &);
	};
}
