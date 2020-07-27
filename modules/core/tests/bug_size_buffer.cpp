/**
 * Netero sources under BSD-3-Clause
 * see LICENSE.txt
 */

#include <netero/buffer.hpp>

#include <gtest/gtest.h>

TEST(NeteroCore, shared_buffer_size_bug)
{
    int                        tmp[5];
    netero::shared_buffer<int> buffer(0);

    EXPECT_EQ(buffer.getPadding(), 0);
    EXPECT_EQ(buffer.read(tmp, 5), 0);
}
