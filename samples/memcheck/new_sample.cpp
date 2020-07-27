/**
 * Netero sources under BSD-3-Clause
 * see LICENSE.txt
 */

#include <iostream>

#include <netero/logger.hpp>
#include <netero/memcheck/new.hpp>

class test {
    public:
    test() { LOG_INFO << ">>> test::ctor" << std::endl; }
    ~test() { LOG_INFO << ">>> test::dtor" << std::endl; }
    int value = 0;
};

int main()
{
    // Memory check is activated while Netero::Memcheck
    // is link to your program and does not required any other configuration

    // activate memory corruption detection do detect overflow at runtime
    netero::memchk::activate_memcorruption_detection();

    // Memory checking is invisible and impact performance as low as possible
    auto* object = new test;
    object->value = 42;
    delete object;

    // To easily check if your program as leaks, call reportLeaks at the end of the program
    auto* myString = new char[256];
    std::memset(myString, 0, 256);

    // Memcheck while detect buffer overflow if you have activated memorycorruption_detection
    auto* nb = new int;
    nb[0] = 42;
    nb[1] = 43;

    delete nb;

    netero::memchk::reportLeaks();

    // For some reason you are allow to deactivate memory corruptions detection
    netero::memchk::deactivate_memcorruption_detection();

    // This module need to be improved to include info such as call stack,...
    return 0;
}
