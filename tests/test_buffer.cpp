/**
 * Netero sources under BSD-3-Clause
 * see LICENCE.txt
 */

#include <netero/buffer.hpp>

int test_full_fill_buffer() {
    int buf[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 };
    netero::shared_buffer<int>  buffer(10);
    buffer.write(buf, 3);
    buffer.write(buf + 3, 3);
    buffer.write(buf + 6, 4);
    if (buffer.write(buf, 3) != 0)
        return 1;
    return 0;
}

int test_full_read() {
    int buf[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 };
    int outBuf[10] = {};
    netero::shared_buffer<int>  buffer(10);
    buffer.write(buf, 10);
    buffer.read(outBuf, 3);
    buffer.read(outBuf + 3, 4);
    buffer.read(outBuf + 7, 3);
    if (buffer.read(outBuf, 10) != 0)
        return 1;
    return 0;
}

int test_faster_read() {
    int buf[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 };
    int outBuf[10] = {};
    netero::shared_buffer<int>  buffer(10);
    if (buffer.write(buf, 3) != 3) {
        return 1;
    }
    if (buffer.read(outBuf, 5) != 3) {
        return 1;
    }
    if (buffer.write(buf+3, 3) != 3) {
        return 1;
    }
    if (buffer.read(outBuf+3, 5) != 3) {
        return 1;
    }
    if (buffer.write(buf+6, 3) != 3) {
        return 1;
    }
    if (buffer.read(outBuf+6, 5) != 3) {
        return 1;
    }
    if (buffer.write(buf+9, 3) != 1) {
        return 1;
    }
    if (buffer.read(outBuf+9, 5) != 1) {
        return 1;
    }
    return 0;
}

int main() {
    int result = test_full_fill_buffer();
    if (result) { return result; }
    result = test_full_read();
    if (result) { return result; }
    result = test_faster_read();
    if (result) { return result; }
    return 0;
}

