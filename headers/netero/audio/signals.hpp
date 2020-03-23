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
#include <netero/audio/device.hpp>

namespace netero::audio::signals {
	class sinusoid: public AudioStream {
	public:
		sinusoid(double amplitude = 0.1, double frequency = 50, double phase = 0);
		virtual ~sinusoid();

		void    setFormat(WaveFormat&) override;
		void    render(float *buffer, size_t size) override;
		void    play() override;
		void    pause() override;
		void    stop() override;

	private:
		float _amplitude;
		float _frequency;
		float _phase;
		float _samplingFrequency;
		float _pan;
		float _pulsation;
		float _delta;
		size_t _samplesCount;
		audio::WaveFormat _format;
		audio::device& _audio_device;

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

