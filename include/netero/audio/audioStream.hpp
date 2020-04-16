/**
 * Netero sources under BSD-3-Clause
 * see LICENSE.txt
 */

#pragma once

/**
 * @file audioStream.hpp
 * @brief AudioStream abstract class.
 */

#include <netero/audio/audio.hpp>

namespace netero::audio {

	/**
	 * @brief AudioStream Abstract class.
	 * This class contain necessary slots and virtual method
	 * to interact with hardware audio device.
	 */
	class AudioStream {
	public:
		AudioStream() noexcept = default;
		AudioStream(const AudioStream&) = delete;
		AudioStream(AudioStream&&) = delete;
		AudioStream& operator=(const AudioStream&) = delete;
		AudioStream& operator=(AudioStream&&) = delete;
		virtual ~AudioStream() = default;

		netero::audio::RenderSlot renderSlot; /**< Slot used for rendering a signal. */
		netero::audio::CaptureSlot captureSlot; /** Slot used to capture a signal. */
		netero::audio::DeviceErrorSlot deviceErrorSlot; /**< Slot used to notify the user a device error. */
		netero::audio::OnDisconnectedSlot deviceDisconnectedSlot; /**< Slot used to notify the user that the device has been disconnected. */
		netero::audio::OnFormatChangeSlot formatChangedSlot; /**< Slot used to notify the user that the stream format has changed. */

		/**
		 * @brief Set the device stream format.
		 * This method should make necessary update in your container
		 * while the format is changer or set for the first time.
		 */
		virtual void setFormat(const netero::audio::StreamFormat& streamFormat) = 0;
	};
}

