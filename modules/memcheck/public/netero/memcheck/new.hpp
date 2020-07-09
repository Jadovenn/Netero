/**
 * Netero sources under BSD-3-Clause
 * see LICENSE.txt
 */

#pragma once

#include <exception>

// Not necessary
void* operator new(std::size_t);
void  operator delete(void*) noexcept;
void* operator new[](std::size_t);
void  operator delete[](void*) noexcept;

namespace netero::memchk {
void reportLeaks();
void activate_memcorruption_detection();
void deactivate_memcorruption_detection();
} // namespace netero::memchk
