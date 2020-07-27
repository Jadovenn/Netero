/**
 * Netero sources under BSD-3-Clause
 * see LICENSE.txt
 */

#pragma once

#if !defined(NDEBUG)

#if defined(_MSC_VER)

#define BREAK __debugbreak()

#elif defined(__has_builtin)

#if __has_builtin(__builtin_debugtrap)
#define BREAK __builtin_debugtrap()
#else // ELSE __has_builtin
#define BREAK __asm__("int $03")
#endif // __builtin debug trap
#else  // unknown (probably CI)
#define BREAK
#endif // Compilers
#else  // ELSE NDEBUG

#define BREAK

#endif // NDEBUG
