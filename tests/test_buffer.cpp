/**
 * Netero sources under BSD-3-Clause
 * see LICENSE.txt
 */

#include <cassert>
#include <netero/buffer.hpp>

void test_full_fill_buffer() {
    int buf[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 };
    netero::shared_buffer<int>  buffer(10);
    buffer.write(buf, 3);
    buffer.write(buf + 3, 3);
    buffer.write(buf + 6, 4);
    assert(buffer.write(buf, 3) == 0);
}

void test_full_read() {
    int buf[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 };
    int outBuf[10] = {};
    netero::shared_buffer<int>  buffer(10);
    buffer.write(buf, 10);
    buffer.read(outBuf, 3);
    buffer.read(outBuf + 3, 4);
    buffer.read(outBuf + 7, 3);
    assert(buffer.read(outBuf, 10) == 0);
}

void    test_faster_read() {
    int buf[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 };
    int outBuf[10] = {};
    netero::shared_buffer<int>  buffer(10);
    assert(buffer.write(buf, 3) == 3);
    assert(buffer.read(outBuf, 5) == 3);
    assert(buffer.write(buf + 3, 3) == 3);
    assert(buffer.read(outBuf + 3, 5) == 3);
    assert(buffer.write(buf + 6, 3) == 3);
    assert(buffer.read(outBuf + 6, 5) == 3);
    assert(buffer.write(buf + 9, 3) == 1);
    assert(buffer.read(outBuf + 9, 5) == 1);
}

void    test_copy_ctor() {
    netero::shared_buffer<int>  buffer(10);
    const netero::shared_buffer<int>  copy(buffer);
    assert(copy.getSize() == 10);
    assert(buffer.getSize() == 10);
}

void    test_move_operators() {
    netero::shared_buffer<int>  buffer(10);
    netero::shared_buffer<int>  move(std::move(buffer));

    assert(move.getSize() == 10);
    assert(buffer.getSize() == 0);

    netero::shared_buffer<int>  moveAssign(10);
	moveAssign = std::move(move);
    assert(moveAssign.getSize() == 10);
    assert(move.getSize() == 0);
}

void    test_comparison_and_test_operators() {
    netero::shared_buffer<int>  buffer(10);
    const netero::shared_buffer<int>  move(std::move(buffer));

    assert(!(buffer == move));
    assert(static_cast<bool>(move));
    assert(!static_cast<bool>(buffer));
}

void    test_reset_and_clear() {
    netero::shared_buffer<int>  buffer(4);
    int buf[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 };

    assert(buffer.getSize() == 4);
    assert(buffer.getPadding() == 0);
    assert(buffer.write(buf, 4) == 4);
    assert(buffer.getPadding() == 4);
    buffer.reset(10);
    assert(buffer.getSize() == 10);
    assert(buffer.getPadding() == 0);
    assert(buffer.write(buf, 14) == 10);
    assert(buffer.getPadding() == 10);
    buffer.clear();
    assert(buffer.getSize() == 10);
    assert(buffer.getPadding() == 0);	
}

// Let the write offset restart to write from the beginning
void    test_circular_write() {
    int buf[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 };
    int outBuf[10] = {};
    netero::shared_buffer<int>  buffer(10);

    assert(buffer.write(buf, 10) == 10);
    assert(buffer.getPadding() == 10);
    assert(buffer.read(outBuf, 6) == 6);
    assert(buffer.getPadding() == 4);
    assert(buffer.write(buf, 6) == 5);
    assert(buffer.getPadding() == 4);
}

int main() {
    test_copy_ctor();
    test_move_operators();
    test_comparison_and_test_operators();
    test_reset_and_clear();
    test_circular_write();
    test_full_fill_buffer();
    test_full_read();
    test_faster_read();
    return 0;
}
