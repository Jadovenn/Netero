/**
 * Netero sources under BSD-3-Clause
 * see LICENCE.txt
 */

#pragma once

#include <functional>

namespace netero::audio {

	enum RtCode {
		OK
	};

	class IEngine {
	public:
		virtual void	registerCB(std::function<void(float*, size_t)>) = 0;
		virtual RtCode	start() = 0;
		virtual RtCode	stop() = 0;
	};

	IEngine* GetAudioEngine();
}

