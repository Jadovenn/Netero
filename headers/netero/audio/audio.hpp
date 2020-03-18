/**
 * Netero sources under BSD-3-Clause
 * see LICENCE.txt
 */

#pragma once

#include <functional>

namespace netero::audio {

	enum RtCode {
		OK,
		ERR_MISSING_CALLBACK,
		ERR_NATIVE,
	};

	class IEngine {
	public:
		virtual void	registerCB(std::function<void(float*, size_t)>) = 0;
		virtual RtCode	start() = 0;
		virtual RtCode	stop() = 0;
	};

	IEngine* GetAudioEngine();
}

