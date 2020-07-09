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

void test_mem_corruption()
{
    netero::memchk::activate_memcorruption_detection();
    auto* nb = new int;
    nb[0] = 42;
    nb[1] = 43;
    delete nb;
    netero::memchk::deactivate_memcorruption_detection();
}

void test_ctor_call()
{
    auto* object = new test;
    object->value = 42;
    delete object;
}

int main()
{
    test_ctor_call();
    test_mem_corruption();
    return 0;
}
