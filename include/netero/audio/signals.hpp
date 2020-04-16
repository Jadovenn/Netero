/**
 * Netero sources under BSD-3-Clause
 * see LICENSE.txt
 */

#pragma once

#include <netero/netero.hpp>
#include <netero/audio/audio.hpp>
#include <netero/audio/renderStream.hpp>

namespace netero::audio::signals {
	class sinusoid: public RenderStream {
	public:
		/**
		 * @brief sinusoid constructor.
		 * @param amplitude of the signal.
		 * @param frequency of the signal.
		 * @param phase offset of the signal.
		 */
		explicit sinusoid(float amplitude = 0.1,
			float frequency = 50,
			float phase = 0);
		sinusoid(const sinusoid&) = delete;
		sinusoid(sinusoid&&) = delete;
		sinusoid& operator=(const sinusoid&) = delete;
		sinusoid& operator=(sinusoid&&) = delete;
		virtual ~sinusoid();

		/**
		 * Apply container configuration from the device stream format.
		 */
		void	setFormat(const netero::audio::StreamFormat& streamFormat) final;

		/**
		 * Render the signal into the given buffer.
		 * @param buffer where to write data.
		 * @param frames available in the buffer.
		 */
		void    renderStream(float *buffer, size_t frames) final;

		/**
		 * Start to produce a signal.
		 */
		void    play() final;

		/**
		 * As the same effect as stop the signal. Stop the
		 * signal generation.
		 */
		void    pause() final;

		/**
		 * Stop the signal generation.
		 */
		void    stop() final;

	private:
		float _amplitude; /**< Amplitude to use to render the signal. */
		float _actualAmplitude; /**< Actual amplitude used for rendering. */
		float _frequency; /**< Frequency of the signal. */
		float _phase; /**< Phase of the signal. Probably unused. */
		float _pulsation; /**< Pre-calculated pulsation using the device frequency.  */
		float _delta; /**< Delay that compute delta time reset. */
		audio::StreamFormat _format; /**< Stream format. */

	public:
		void			setAmplitude(const float val) { _amplitude = val; }
		[[nodiscard]] float	getAmplitude() const { return _amplitude; }
		void			setFrequency(const float val) {
			_frequency = val;
			_pulsation = (2 * netero::numbers::pi * _frequency) / _format.samplingFrequency;
		}
		[[nodiscard]] float	getFrequency() const { return _frequency; }
		void			setPhase(const float val) { _phase = val; }
		[[nodiscard]] float	getPhase() const { return _phase; }
		[[nodiscard]] float	getSamplingFrequency() const { return _format.samplingFrequency; }
	};
}

