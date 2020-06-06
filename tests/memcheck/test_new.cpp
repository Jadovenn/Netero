/**
 * Netero sources under BSD-3-Clause
 * see LICENSE.txt
 */

#include <iostream>
#include <netero/memcheck/new.hpp>

class test {
public:
    test() { std::cout << ">>> test::ctor" << std::endl; }
    ~test() { std::cout << ">>> test::dtor" << std::endl; }
    int value = 0;
};

void test_overflow() {
    auto* nb = new int;
    nb[0] = 42;
    nb[1] = 43;
    delete nb;
}

void test_class() {
    auto* object = new test;
    object->value = 42;
    delete object;
}

int main() {
    test_overflow();
    test_class();
    return 0;
}

