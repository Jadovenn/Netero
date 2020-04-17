/**
 * Netero sources under BSD-3-Clause
 * see LICENSE.txt
 */

#pragma once

#include <memory>
#include <functional>
#include <netero/audio/audio.hpp>
#include <netero/audio/device.hpp>

 /**
  * @file backend.hpp
  * @brief Declaration for audio backend implementation
  * You should never refer directly to any backend container.
  * @see netero::audio::engine
  */

namespace netero::audio {

	/**
	 * @brief backend
	 * The backend class is the API to implement
	 * for any specific operating system audio API.
	 */
	class backend {
	public:
		/**
		 * @brief Well... This return a reference to the audio engine instance.
		 */
		static backend& GetInstance();
		
		backend(const backend&) = delete;
		backend(backend&&) = delete;
		backend& operator=(const backend&) = delete;
		backend& operator=(backend&&) = delete;
		~backend();

		/**
		 * @brief Get available rendering device.
		 * @info This should look for any new connected device at each call.
		 */
		[[nodiscard]] std::vector<device>			getRenderDevices() const;

		/**
		 * @brief Get the default rendering device. 
		 */
		[[nodiscard]] device&						getDefaultRenderDevice() const;

		/**
		 * @brief Get available capture device.
		 * @info This should look for any new connected device at each call.
		 */
		[[nodiscard]] std::vector<device>			getCaptureDevices() const;

		/**
		 * @brief Get the default capture device.
		 */
		[[nodiscard]] device&						getDefaultCaptureDevice() const;

		/**
		 * @brief Initialize resources and start the rendering procedure for the given device.
		 */
		RtCode						deviceStartRendering(const device&);

		/**
		 * @brief Uninitialized resources and stop the rendering procedure for the given device.
		 */
		RtCode						deviceStopRendering(const device&);

		/**
		 * @brief Initialize resources and start the capture procedure for the given device.
		 */
		RtCode						deviceStartRecording(const device&);

		/**
		 * @brief Uninitialized resources and stop the capture procedure for the given device.
		 */
		RtCode						deviceStopRecording(const device&);

		/**
		 * @brief return the last error message from the hardware devices.
		 */
		[[nodiscard]] const std::string&			getLastError() const;

		/**
		 * @brief Save a callback to call while a device disconnection is detected.
		 */
		void	setDeviceDisconnectedCallback(const std::function<void(const netero::audio::device&)>&) const;
	private:

		/**
		 * @brief Private ctor is implemented in impl sources.
		 */
		backend();
		class impl;
		std::unique_ptr<impl>	pImpl; /** Pointer to specific OS implementation. */
	};
}

