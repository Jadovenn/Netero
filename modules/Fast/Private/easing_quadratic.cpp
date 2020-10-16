/**
 * Netero sources under BSD-3-Clause
 * see LICENSE.txt
 */

#include <Netero/Fast/Easing.hpp>

namespace Netero::Fast {

float Quad(float k)
{
    return k * k;
}

double Quad(double k)
{
    return k * k;
}

__m128d Quad(__m128d k)
{
    return _mm_mul_pd(k, k);
}

__m128 Quad_f(__m128 k)
{
    return _mm_mul_ps(k, k);
}

void Quad(float* data, const size_t size)
{
    unsigned idx = 0;
    while ((size - idx) % 4) {
        data[idx] = data[idx] * data[idx];
        idx += 1;
    }
    while (idx < size) {
        __m128 xm_reg = _mm_load_ps(data + idx);
        xm_reg = _mm_mul_ps(xm_reg, xm_reg);
        _mm_store_ps(data + idx, xm_reg);
        idx += 4;
    }
}

} // namespace Netero::Fast
