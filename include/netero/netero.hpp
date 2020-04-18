/**
 * Netero sources under BSD-3-Clause
 * see LICENSE.txt
 */

#pragma once

/**
 * @file netero.hpp
 * @brief Include all netero core features.
 */

#include <netero/type_id.hpp>
#include <netero/observer/signal.hpp>
#include <netero/observer/signal.hpp>

/**
 * @brief Netero namespace.
 */
namespace netero {
	constexpr char const *version = "0.0.1"; /**< Version string. */

	/**
	 * @brief Constant numbers namespace.
	 */
	namespace numbers {
        constexpr float pi = 3.14159265358979323846; /**< PI as float. */
	}
}

