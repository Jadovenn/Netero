/**
 * Netero sources under BSD-3-Clause
 * see LICENCE.txt
 */

#pragma once

/**
 * @file netero.hpp
 * @brief Include all netero core features.
 */

#include <netero/core/internal.hpp>
#include <netero/core/type_id.hpp>

#include <netero/observer/signal.hpp>
#include <netero/observer/slot.hpp>

/**
 * @brief Netero namespace.
 */
namespace netero {
	constexpr char const *version = "0.0.1"; /**< Version string. */
}
