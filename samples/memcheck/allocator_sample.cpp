/**
 * Netero sources under BSD-3-Clause
 * see LICENSE.txt
 */

#include <iostream>
#include <list>
#include <vector>

#include <Netero/Avl.hpp>
#include <Netero/Debug.hpp>
#include <Netero/Logger.hpp>
#include <Netero/MemCheck/Allocator.hpp>

class test {
    public:
    test() { LOG_INFO << ">>> test::ctor" << std::endl; }
    ~test() { LOG_INFO << ">>> test::dtor" << std::endl; }

    int value = 0;
};

// Check if it compiles with standards container
void stl_container_test()
{
    std::vector<int, Netero::Memck::Allocator<int>> myVector;
    myVector.push_back(42);
    myVector.resize(100);
    myVector.shrink_to_fit();
    myVector.pop_back();
    std::list<float, Netero::Memck::Allocator<float>> myList;
    myList.push_back(42.f);
    myList.pop_back();
}

// Check if very basic alloc works (expect no segfault)
void on_class_test()
{
    Netero::Memck::Allocator<test> testAllocator;
    auto*                          object =
        std::allocator_traits<Netero::Memck::Allocator<test>>::allocate(testAllocator, 1);
    std::allocator_traits<Netero::Memck::Allocator<test>>::construct(testAllocator, object);
    object->value = 42;
    std::allocator_traits<Netero::Memck::Allocator<test>>::destroy(testAllocator, object);
    std::allocator_traits<Netero::Memck::Allocator<test>>::deallocate(testAllocator, object, 1);
}

// Check if rebind compile
void rebind_test()
{
    Netero::Memck::Allocator<int>                      allocator;
    Netero::Memck::Allocator<int>::rebind<char>::other rebindAllocator;
    auto* integer = std::allocator_traits<Netero::Memck::Allocator<int>>::allocate(allocator, 1);
    auto* character =
        std::allocator_traits<Netero::Memck::Allocator<int>::rebind<char>::other>::allocate(
            rebindAllocator,
            1);
    *integer = 42;
    *character = 'A';
    std::allocator_traits<Netero::Memck::Allocator<int>::rebind<char>::other>::deallocate(
        rebindAllocator,
        character,
        1);
    std::allocator_traits<Netero::Memck::Allocator<int>>::deallocate(allocator, integer, 1);
}

// Check if it compile with a netero container
void avl_test()
{
    Netero::Avl<int, Netero::Memck::Allocator<int>> memcheckAvl;
    memcheckAvl.Insert(5);
    memcheckAvl.Insert(2);
    memcheckAvl.Insert(1);
    memcheckAvl.Insert(12);
    memcheckAvl.Insert(24);
    memcheckAvl.Insert(8);
    memcheckAvl.Insert(3);
    memcheckAvl.Insert(4);
    memcheckAvl.Find(1);
    memcheckAvl.Find(23);
    memcheckAvl.Remove(1);
    memcheckAvl.Find(1);
    memcheckAvl.Remove(2);
    memcheckAvl.Remove(12);
}

template<typename Allocator>
void wrong_behavior()
{
    Netero::Memck::Allocator<int> allocator;
    auto* null = std::allocator_traits<Netero::Memck::Allocator<int>>::allocate(allocator, 0);
    auto* integer = std::allocator_traits<Netero::Memck::Allocator<int>>::allocate(allocator, 1);
}

int main()
{
    stl_container_test();
    on_class_test();
    rebind_test();
    avl_test();
    wrong_behavior<Netero::Memck::Allocator<int>>();
    wrong_behavior<Netero::Memck::LoggerAllocator<int>>();
}
