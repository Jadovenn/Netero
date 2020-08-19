
#ifndef OVERSAMPLING_H_
#define OVERSAMPLING_H_

#ifdef __cplusplus
namespace netero {
extern "C" {
#endif // __cplusplus

void OverSampling(float __restrict *bufIn, float __restrict *bufOut, size_t size, float ratio);

#ifdef __cplusplus
}
}
#endif // __cplusplus

#endif // OVERSAMPLING_H_
