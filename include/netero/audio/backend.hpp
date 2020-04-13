/**
 * Netero sources under BSD-3-Clause
 * see LICENCE.txt
 */

#pragma once

#include <memory>
#include <functional>

 /**
  * @file backend.hpp
  * @brief Declaration for audio backend implementation
  * You should never refer directly to any backend container.
  * @see netero::audio::engine
  */

#include <netero/audio/audio.hpp>
#include <netero/audio/device.hpp>

namespace netero::audio {

	/**
	 * @brief backend
	 * The backend class is the API to implement
	 * for any specific operating system audio API.
	 */
	class backend {
	public:
		static backend& GetInstance();
		~backend();

		const std::vector<device>	getRenderDevices();
		const device&				getDefaultRenderDevice();
		const std::vector<device>	getCaptureDevices();
		const device&				getDefaultCaptureDevice();

		device::events& getDeviceEvents(const device&); // may throw

		RtCode						deviceStartRendering(const device&);
		RtCode						deviceStopRendering(const device&);
		RtCode						deviceStartRecording(const device&);
		RtCode						deviceStopRecording(const device&);
		const std::string&			getLastError();

		void	setDeviceDisconnectedCallback(const std::function<void(const netero::audio::device&)>&) const;
	private:
		backend();
		class impl;
		std::unique_ptr<impl>	pImpl;
	};
}

