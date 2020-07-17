/**
 * Netero sources under BSD-3-Clause
 * see LICENSE.txt
 */

#include <chrono>
#include <iostream>

#include <netero/fast/easing.hpp>

#include <gtest/gtest.h>

TEST(NeteoFast, test_quad_double)
{
    double  values[] = { 0.2, 0.3 };
    __m128d xm_register = _mm_load_pd(values);
    xm_register = netero::fast::quad(xm_register);
    _mm_store_pd(values, xm_register);
    EXPECT_GT(0.1, values[0]);
    EXPECT_GT(0.1, values[1]);
}

TEST(NeterFast, test_quad_vector)
{
    float values[] = { 0.1, 0.2, 0.3, 0.4, 0.5, 0.6, 0.7, 0.8 };
    netero::fast::quad(values, 8);
    EXPECT_GT(0.1, values[0]);
    EXPECT_GT(0.1, values[1]);
    EXPECT_GT(0.1, values[2]);
    EXPECT_GT(0.2, values[3]);
    EXPECT_GT(0.3, values[4]);
    EXPECT_GT(0.4, values[5]);
    EXPECT_GT(0.5, values[6]);
    EXPECT_GT(0.7, values[7]);
}

TEST(NeteroFast, compute_quad_speed)
{
    // Without SIMD
    float *tab = new float[8192];
    auto   start = std::chrono::high_resolution_clock::now();
    for (unsigned idx = 0; idx < 8192; ++idx)
        tab[idx] = netero::fast::quad(tab[idx]);
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
    std::cout << "\033[0;32m"
              << "[     cout ] "
              << "\033[0;0m"
              << "Classic takes: " << duration << " microseconds." << std::endl;
    delete[] tab;

    // With SIMD
    tab = new float[8192];
    start = std::chrono::high_resolution_clock::now();
    netero::fast::quad(tab, 8192);
    end = std::chrono::high_resolution_clock::now();
    auto duration_SIMD = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
    std::cout << "\033[0;32m"
              << "[     cout ] "
              << "\033[0;0m"
              << "SIMD takes: " << duration_SIMD << " microseconds." << std::endl;
    delete[] tab;
    EXPECT_GT(duration, duration_SIMD);
}

int main(int argc, char **argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
