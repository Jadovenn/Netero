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

		const std::vector<device>&	getRenderDevices();
		RtCode						setRenderDevice(const device&);
		WaveFormat					getRenderFormat();
		RtCode						startRender();
		RtCode						stopRender();
		RtCode						setRenderCallback(const RenderCallback&);

		const std::vector<device>&	getCaptureDevices();
		RtCode						setCaptureDevice(const device&);
		WaveFormat					getCaptureFormat();
		RtCode						capturingThreadHandle();
		RtCode						startCapture();
		RtCode						stopCapture();
		RtCode						setCaptureErrorCallback(const std::function<void(const std::string&)>&);
		RtCode						setCaptureCallback(const CaptureCallback &);
	};
}
