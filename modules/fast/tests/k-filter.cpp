
#include <emmintrin.h>

int main()
{
    __m128i a = { 1, 1, 1, 1, 2 };
    __m128i b = { 2, 2, 2, 2 };

    __m128i result = _mm_add_epi32(a, b);
    int     someValue = _mm_cvtsi128_si32(result);
}
