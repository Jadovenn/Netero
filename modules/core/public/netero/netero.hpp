/**
 * Netero sources under BSD-3-Clause
 * see LICENSE.txt
 */

#pragma once

/**
 * @file netero.hpp
 * @brief Include all netero core features.
 */

/**
 * @brief Netero namespace.
 */
namespace netero {
constexpr char const *version = "0.0.1"; /**< Version string. */

#if !defined(NDEBUG)
constexpr bool isDebugMode = true;
#else
constexpr bool isDebugMode = false; /**< Debug mode flag. Evaluate to true if NDEBUG is defined. */
#endif // NDEBUG

/**
 * @brief Constant numbers namespace.
 */
namespace numbers {
    constexpr float pi = 3.14159265358979323846F; /**< Pi constant. */
}
} // namespace netero

#if defined __GNUC__ || defined __clang__
#define likely(x)   __builtin_expect(!!(x), 1)
#define unlikely(x) __builtin_expect(!!(x), 0)
#else
#define likely(x)   (x)
#define unlikely(x) (x)
#endif
