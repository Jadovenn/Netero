/**
 * Netero sources under BSD-3-Clause
 * see LICENCE.txt
 */

#pragma once

#define _USE_MATH_DEFINES

#include <cmath>
#include <type_traits>
#include <atomic>
#include <netero/audio/audio.hpp>
#include <netero/audio/renderStream.hpp>
#include <netero/audio/mixer.hpp>

namespace netero::audio::signals {
	class sinusoid: public RenderStream {
	public:
		explicit sinusoid(netero::audio::mixer& mixer,
			double amplitude = 0.1,
			double frequency = 50,
			double phase = 0);
		virtual ~sinusoid();

		void    onFormatChange(const StreamFormat&) override;
		void    renderStream(float *buffer, size_t frames) override;
		void    play() override;
		void    pause() override;
		void    stop() override;

	private:
		netero::audio::mixer& _mixer;
		float _amplitude;
		float _frequency;
		float _phase;
		float _samplingFrequency;
		float _pan;
		float _pulsation;
		float _delta;
		size_t _samplesCount;
		audio::StreamFormat _format;

	public:
		void			setAmplitude(double val) { _amplitude = val; }
		const double	getAmplitude() const { return _amplitude; }
		void			setFrequency(double val) {
			_frequency = val;
			_pulsation = ((2 * M_PI * _frequency) / _samplingFrequency);
		}
		const double	getFrequency() const { return _frequency; }
		void			setPhase(double val) { _phase = val; }
		const double	getPhase() const { return _phase; }
		const double	getSamplingFrequency() const { return _samplingFrequency; }
		void			setPan(double val) { _pan = val; }
		const double	getPan() const { return _pan; }
	};
}

