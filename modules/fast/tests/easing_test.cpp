/**
 * Netero sources under BSD-3-Clause
 * see LICENSE.txt
 */

#include <chrono>
#include <iostream>

#include <netero/fast/easing.hpp>

void test_quad_double()
{
    double values[] = { 0.2, 0.3 };
    std::cout << values[0] << " : " << values[1] << std::endl;
    __m128d xm_register = _mm_load_pd(values);
    xm_register = netero::fast::quad(xm_register);
    _mm_store_pd(values, xm_register);
    std::cout << values[0] << " : " << values[1] << std::endl;
}

void test_quad_vector()
{
    float values[] = { 0.1, 0.2, 0.3, 0.4, 0.5, 0.6, 0.7, 0.8 };
    for (unsigned idx = 0; idx < 8; ++idx)
        std::cout << values[idx] << " : ";
    std::cout << std::endl;
    netero::fast::quad(values, 8);
    for (unsigned idx = 0; idx < 8; ++idx)
        std::cout << values[idx] << " : ";
    std::cout << std::endl;
}

void perf_quad_classic()
{
    float *tab = new float[8192];
    auto   start = std::chrono::high_resolution_clock::now();
    for (unsigned idx = 0; idx < 8192; ++idx)
        tab[idx] = netero::fast::quad(tab[idx]);
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
    std::cout << "Classic takes: " << duration << " microseconds." << std::endl;
    delete[] tab;
}

void perf_quad_simd()
{
    float *tab = new float[8192];
    auto   start = std::chrono::high_resolution_clock::now();
    netero::fast::quad(tab, 8192);
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
    std::cout << "SIMD takes: " << duration << " microseconds." << std::endl;
    delete[] tab;
}

int main()
{
    test_quad_double();
    test_quad_vector();
    perf_quad_classic();
    perf_quad_simd();
    return 0;
}
