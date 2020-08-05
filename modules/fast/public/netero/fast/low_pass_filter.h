/**
 * Netero sources under BSD-3-Clause
 * see LICENSE.txt
 */

#ifndef LOW_PASS_FILTER_H
#define LOW_PASS_FILTER_H

#include <stddef.h>

#ifdef __cplusplus
namespace netero {
extern "C" {
#endif // __cplusplus

void LowPassFilter(float * __restrict buffIn, float* __restrict buffOut, size_t length, float freqIn, float freqOut);

#ifdef __cplusplus
};
}
#endif // __cplusplus

#endif // LOW_PASS_FILTER_H
