/**
 * Netero sources under BSD-3-Clause
 * see LICENSE.txt
 */

#include <chrono>
#include <cstring>
#include <iostream>

#include <netero/extra/stb_image_write.h>
#include <netero/fast/mandlebrot.hpp>

#include <gtest/gtest.h>

TEST(NeteroFast, mandlebrotTest)
{
    int *output = new int[1920 * 1080];
    netero::fast::fractal::ComputeMandle(-2, 1, -1, 1, 1920, 1080, 256, output);
    stbi_write_png("mandlebrot.png", 1920, 1080, 4, output, 1920 * 4);
    delete[] output;
}
