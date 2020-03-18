/**
 * Netero sources under BSD-3-Clause
 * see LICENCE.txt
 */

#pragma once

#define _USE_MATH_DEFINES
#include <cmath>
#include <type_traits>

namespace netero::signals {

	template<typename T = float,
			typename = std::enable_if<std::is_convertible<T, double>::value>>
	class Sinusoidal {
	public:
		Sinusoidal(T amplitude, T pulsation, T freq, T phase)
			: amplitude(amplitude),
			pulsation((2 * M_PI * pulsation) / freq),
			phase(phase)
		{}

		T	operator()(T t) {
			return amplitude * sin(pulsation * t + phase);
		}

		T amplitude;
		T pulsation;
		T phase;
	};
}

