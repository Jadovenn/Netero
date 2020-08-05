/**
 * Netero sources under BSD-3-Clause
 * see LICENSE.txt
 */

#include <iostream>
#include <list>
#include <vector>

#include <netero/avl.hpp>
#include <netero/debug.hpp>
#include <netero/logger.hpp>
#include <netero/memcheck/allocator.hpp>

class test {
    public:
    test() { LOG_INFO << ">>> test::ctor" << std::endl; }
    ~test() { LOG_INFO << ">>> test::dtor" << std::endl; }

    int value = 0;
};

// Check if it compiles with standards container
void stl_container_test()
{
    std::vector<int, netero::memck::Allocator<int>> myVector;
    myVector.push_back(42);
    myVector.resize(100);
    myVector.shrink_to_fit();
    myVector.pop_back();
    std::list<float, netero::memck::Allocator<float>> myList;
    myList.push_back(42.f);
    myList.pop_back();
}

// Check if very basic alloc works (expect no segfault)
void on_class_test()
{
    netero::memck::Allocator<test> testAllocator;
    auto*                          object =
        std::allocator_traits<netero::memck::Allocator<test>>::allocate(testAllocator, 1);
    std::allocator_traits<netero::memck::Allocator<test>>::construct(testAllocator, object);
    object->value = 42;
    std::allocator_traits<netero::memck::Allocator<test>>::destroy(testAllocator, object);
    std::allocator_traits<netero::memck::Allocator<test>>::deallocate(testAllocator, object, 1);
}

// Check if rebind compile
void rebind_test()
{
    netero::memck::Allocator<int>                      allocator;
    netero::memck::Allocator<int>::rebind<char>::other rebindAllocator;
    auto* integer = std::allocator_traits<netero::memck::Allocator<int>>::allocate(allocator, 1);
    auto* character =
        std::allocator_traits<netero::memck::Allocator<int>::rebind<char>::other>::allocate(
            rebindAllocator,
            1);
    *integer = 42;
    *character = 'A';
    std::allocator_traits<netero::memck::Allocator<int>::rebind<char>::other>::deallocate(
        rebindAllocator,
        character,
        1);
    std::allocator_traits<netero::memck::Allocator<int>>::deallocate(allocator, integer, 1);
}

// Check if it compile with a netero container
void avl_test()
{
    netero::avl<int, netero::memck::Allocator<int>> memcheckAvl;
    memcheckAvl.add(5);
    memcheckAvl.add(2);
    memcheckAvl.add(1);
    memcheckAvl.add(12);
    memcheckAvl.add(24);
    memcheckAvl.add(8);
    memcheckAvl.add(3);
    memcheckAvl.add(4);
    memcheckAvl.search(1);
    memcheckAvl.search(23);
    memcheckAvl.remove(1);
    memcheckAvl.search(1);
    memcheckAvl.remove(2);
    memcheckAvl.remove(12);
}

template<typename Allocator>
void wrong_behavior()
{
    netero::memck::Allocator<int> allocator;
    auto* null = std::allocator_traits<netero::memck::Allocator<int>>::allocate(allocator, 0);
    auto* integer = std::allocator_traits<netero::memck::Allocator<int>>::allocate(allocator, 1);
}

int main()
{
    stl_container_test();
    on_class_test();
    rebind_test();
    avl_test();
    wrong_behavior<netero::memck::Allocator<int>>();
    wrong_behavior<netero::memck::LoggerAllocator<int>>();
}
