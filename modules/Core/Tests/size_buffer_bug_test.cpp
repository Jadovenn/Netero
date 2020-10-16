/**
 * Netero sources under BSD-3-Clause
 * see LICENSE.txt
 */

#include <Netero/Buffer.hpp>

#include <gtest/gtest.h>

TEST(NeteroCore, shared_buffer_size_bug)
{
    int                       tmp[5];
    Netero::SharedBuffer<int> buffer(0);

    EXPECT_EQ(buffer.GetPadding(), 0);
    EXPECT_EQ(buffer.Read(tmp, 5), 0);
}
