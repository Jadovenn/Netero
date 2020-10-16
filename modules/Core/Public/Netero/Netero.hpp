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
namespace Netero {
constexpr char const *Version = "0.0.1"; /**< Version string. */

#if !defined(NDEBUG)
constexpr bool IsDebugMode = true;
#else
constexpr bool IsDebugMode = false; /**< Debug mode flag. Evaluate to true if NDEBUG is defined. */
#endif // NDEBUG

/**
 * @brief Constant numbers namespace.
 */
namespace Numbers {
    constexpr float pi = 3.14159265358979323846F; /**< Pi constant. */
}
} // namespace Netero

#if defined __GNUC__ || defined __clang__
#define likely(x)   __builtin_expect(!!(x), 1)
#define unlikely(x) __builtin_expect(!!(x), 0)
#else
#define likely(x)   (x)
#define unlikely(x) (x)
#endif
