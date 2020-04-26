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

	namespace numbers {
        constexpr float pi = 3.14159265358979323846;
	}
}


#if defined __GNUC__ || defined __clang__
#   define likely(x)   __builtin_expect(!!(x), 1)
#   define unlikely(x) __builtin_expect(!!(x), 0)
#else
#   define likely(x)   (x)
#   define unlikely(x) (x)
#endif

