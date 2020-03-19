/**
 * Netero sources under BSD-3-Clause
 * see LICENCE.txt
 */

#pragma once

#include <functional>

namespace netero::audio {

	enum RtCode {
		OK = 0,
		ERR_MISSING_CALLBACK = 1,
		ERR_NATIVE = 2,
	};
}

