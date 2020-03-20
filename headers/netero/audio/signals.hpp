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

namespace netero::audio::signals {
	class sinusoid: public AudioStream {
	public:
		sinusoid();
		virtual ~sinusoid() = default;

		void    setFormat(WaveFormat&) override;
		double  render(int delta, int channel) override;
		void    play() override;
		void    pause() override;
		void    stop() override;

	private:
		double _amplitude;
		double _frequency;
		double _phase;
		double _samplingFrequency;
		double _pan;
		double _pulsation;
		std::atomic<bool>	_isRunning;
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

