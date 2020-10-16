/**
 * Netero sources under BSD-3-Clause
 * see LICENSE.txt
 */

#include <Netero/Buffer.hpp>

#include <gtest/gtest.h>

TEST(NeteroCore, shared_buffer_full_fill)
{
    int                       buf[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 };
    Netero::SharedBuffer<int> buffer(10);
    buffer.Write(buf, 3);
    buffer.Write(buf + 3, 3);
    buffer.Write(buf + 6, 4);
    EXPECT_EQ(buffer.Write(buf, 3), 0);
}

TEST(NeteroCore, shared_buffer_full_read)
{
    int                       buf[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 };
    int                       outBuf[10] = {};
    Netero::SharedBuffer<int> buffer(10);
    buffer.Write(buf, 10);
    buffer.Read(outBuf, 3);
    buffer.Read(outBuf + 3, 4);
    buffer.Read(outBuf + 7, 3);
    EXPECT_EQ(buffer.Read(outBuf, 10), 0);
}

TEST(NeteroCore, shared_buffer_faster_read)
{
    int                       buf[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 };
    int                       outBuf[10] = {};
    Netero::SharedBuffer<int> buffer(10);
    EXPECT_EQ(buffer.Write(buf, 3), 3);
    EXPECT_EQ(buffer.Read(outBuf, 5), 3);
    EXPECT_EQ(buffer.Write(buf + 3, 3), 3);
    EXPECT_EQ(buffer.Read(outBuf + 3, 5), 3);
    EXPECT_EQ(buffer.Write(buf + 6, 3), 3);
    EXPECT_EQ(buffer.Read(outBuf + 6, 5), 3);
    EXPECT_EQ(buffer.Write(buf + 9, 3), 1);
    EXPECT_EQ(buffer.Read(outBuf + 9, 5), 1);
}

TEST(NeteroCore, shared_buffer_copy_ctor)
{
    Netero::SharedBuffer<int>       buffer(10);
    const Netero::SharedBuffer<int> copy(buffer);
    EXPECT_EQ(copy.GetSize(), 10);
    EXPECT_EQ(buffer.GetSize(), 10);
}

TEST(NeteroCore, shared_buffer_move_operators)
{
    Netero::SharedBuffer<int> buffer(10);
    Netero::SharedBuffer<int> move(std::move(buffer));

    EXPECT_EQ(move.GetSize(), 10);
    EXPECT_EQ(buffer.GetSize(), 0);

    Netero::SharedBuffer<int> moveAssign(10);
    moveAssign = std::move(move);
    EXPECT_EQ(moveAssign.GetSize(), 10);
    EXPECT_EQ(move.GetSize(), 0);
}

TEST(NeteroCore, shared_buffer_comparison_and_mote)
{
    Netero::SharedBuffer<int>       buffer(10);
    const Netero::SharedBuffer<int> move(std::move(buffer));

    EXPECT_FALSE(buffer == move);
    EXPECT_TRUE(static_cast<bool>(move));
    EXPECT_FALSE(static_cast<bool>(buffer));
}

TEST(NeteroCore, shared_buffer_reset_and_clear)
{
    Netero::SharedBuffer<int> buffer(4);
    int                       buf[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 };

    EXPECT_EQ(buffer.GetSize(), 4);
    EXPECT_EQ(buffer.GetPadding(), 0);
    EXPECT_EQ(buffer.Write(buf, 4), 4);
    EXPECT_EQ(buffer.GetPadding(), 4);
    buffer.Resize(10);
    EXPECT_EQ(buffer.GetSize(), 10);
    EXPECT_EQ(buffer.GetPadding(), 0);
    EXPECT_EQ(buffer.Write(buf, 14), 10);
    EXPECT_EQ(buffer.GetPadding(), 10);
    buffer.Clear();
    EXPECT_EQ(buffer.GetSize(), 10);
    EXPECT_EQ(buffer.GetPadding(), 0);
}

// Let the write offset restart to write from the beginning
TEST(NeteroCore, shared_buffer_circular_write)
{
    int                       buf[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 };
    int                       outBuf[10] = {};
    Netero::SharedBuffer<int> buffer(10);

    EXPECT_EQ(buffer.Write(buf, 10), 10);
    EXPECT_EQ(buffer.GetPadding(), 10);
    EXPECT_EQ(buffer.Read(outBuf, 6), 6);
    EXPECT_EQ(buffer.GetPadding(), 4);
    EXPECT_EQ(buffer.Write(buf, 6), 5);
    EXPECT_EQ(buffer.GetPadding(), 4);
}
