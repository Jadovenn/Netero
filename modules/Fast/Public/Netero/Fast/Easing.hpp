/**
 * Netero sources under BSD-3-Clause
 * see LICENSE.txt
 */

#pragma once

#include <Netero/Fast/Simd.hpp>

namespace Netero::Fast {

float   Quad(float);
double  Quad(double);
__m128d Quad(__m128d);
__m128  Quad_f(__m128);

void Quad(float*, const size_t);

} // namespace Netero::Fast
