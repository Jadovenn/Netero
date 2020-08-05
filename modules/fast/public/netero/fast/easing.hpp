/**
 * Netero sources under BSD-3-Clause
 * see LICENSE.txt
 */

#pragma once

#include <netero/fast/simd.hpp>

namespace netero::fast {

float   quad(float);
double  quad(double);
__m128d quad(__m128d);
__m128  quand_f(__m128);

void quad(float*, const size_t);

} // namespace netero::fast
