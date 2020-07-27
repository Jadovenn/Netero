/**
 * Netero sources under BSD-3-Clause
 * see LICENSE.txt
 */

#include <netero/buffer.hpp>

#include <gtest/gtest.h>

TEST(NeteroCore, shared_buffer_full_fill)
{
    int                        buf[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 };
    netero::shared_buffer<int> buffer(10);
    buffer.write(buf, 3);
    buffer.write(buf + 3, 3);
    buffer.write(buf + 6, 4);
    EXPECT_EQ(buffer.write(buf, 3), 0);
}

TEST(NeteroCore, shared_buffer_full_read)
{
    int                        buf[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 };
    int                        outBuf[10] = {};
    netero::shared_buffer<int> buffer(10);
    buffer.write(buf, 10);
    buffer.read(outBuf, 3);
    buffer.read(outBuf + 3, 4);
    buffer.read(outBuf + 7, 3);
    EXPECT_EQ(buffer.read(outBuf, 10), 0);
}

TEST(NeteroCore, shared_buffer_faster_read)
{
    int                        buf[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 };
    int                        outBuf[10] = {};
    netero::shared_buffer<int> buffer(10);
    EXPECT_EQ(buffer.write(buf, 3), 3);
    EXPECT_EQ(buffer.read(outBuf, 5), 3);
    EXPECT_EQ(buffer.write(buf + 3, 3), 3);
    EXPECT_EQ(buffer.read(outBuf + 3, 5), 3);
    EXPECT_EQ(buffer.write(buf + 6, 3), 3);
    EXPECT_EQ(buffer.read(outBuf + 6, 5), 3);
    EXPECT_EQ(buffer.write(buf + 9, 3), 1);
    EXPECT_EQ(buffer.read(outBuf + 9, 5), 1);
}

TEST(NeteroCore, shared_buffer_copy_ctor)
{
    netero::shared_buffer<int>       buffer(10);
    const netero::shared_buffer<int> copy(buffer);
    EXPECT_EQ(copy.getSize(), 10);
    EXPECT_EQ(buffer.getSize(), 10);
}

TEST(NeteroCore, shared_buffer_move_operators)
{
    netero::shared_buffer<int> buffer(10);
    netero::shared_buffer<int> move(std::move(buffer));

    EXPECT_EQ(move.getSize(), 10);
    EXPECT_EQ(buffer.getSize(), 0);

    netero::shared_buffer<int> moveAssign(10);
    moveAssign = std::move(move);
    EXPECT_EQ(moveAssign.getSize(), 10);
    EXPECT_EQ(move.getSize(), 0);
}

TEST(NeteroCore, shared_buffer_comparison_and_mote)
{
    netero::shared_buffer<int>       buffer(10);
    const netero::shared_buffer<int> move(std::move(buffer));

    EXPECT_FALSE(buffer == move);
    EXPECT_TRUE(static_cast<bool>(move));
    EXPECT_FALSE(static_cast<bool>(buffer));
}

TEST(NeteroCore, shared_buffer_reset_and_clear)
{
    netero::shared_buffer<int> buffer(4);
    int                        buf[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 };

    EXPECT_EQ(buffer.getSize(), 4);
    EXPECT_EQ(buffer.getPadding(), 0);
    EXPECT_EQ(buffer.write(buf, 4), 4);
    EXPECT_EQ(buffer.getPadding(), 4);
    buffer.reset(10);
    EXPECT_EQ(buffer.getSize(), 10);
    EXPECT_EQ(buffer.getPadding(), 0);
    EXPECT_EQ(buffer.write(buf, 14), 10);
    EXPECT_EQ(buffer.getPadding(), 10);
    buffer.clear();
    EXPECT_EQ(buffer.getSize(), 10);
    EXPECT_EQ(buffer.getPadding(), 0);
}

// Let the write offset restart to write from the beginning
TEST(NeteroCore, shared_buffer_circular_write)
{
    int                        buf[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 };
    int                        outBuf[10] = {};
    netero::shared_buffer<int> buffer(10);

    EXPECT_EQ(buffer.write(buf, 10), 10);
    EXPECT_EQ(buffer.getPadding(), 10);
    EXPECT_EQ(buffer.read(outBuf, 6), 6);
    EXPECT_EQ(buffer.getPadding(), 4);
    EXPECT_EQ(buffer.write(buf, 6), 5);
    EXPECT_EQ(buffer.getPadding(), 4);
}
