
#include <iostream>
#include <vector>
#include <list>
#include <netero/avl.hpp>
#include <netero/memcheck/allocator.hpp>

class test {
public:
    test() { std::cout << ">>> test::ctor" << std::endl; }
    ~test() { std::cout << ">>> test::dtor" << std::endl; }

    int value = 0;
};

void stl_basic_container_test() {
    std::cout << "stl_basic_container_test -- start" << std::endl;
    std::vector<int, netero::memck::Allocator<int>>  myVector;
    myVector.push_back(42);
    myVector.resize(100);
    myVector.shrink_to_fit();
    myVector.pop_back();
    std::list<float, netero::memck::Allocator<float>> myList;
    myList.push_back(42.f);
    myList.pop_back();
    std::cout << "stl_basic_container_test -- end" << std::endl;
}

void on_class_test() {
    std::cout << "on_class_test -- start" << std::endl;
    netero::memck::Allocator<test>  testAllocator;
    auto* object = std::allocator_traits<netero::memck::Allocator<test>>::allocate(testAllocator, 1);
    std::allocator_traits<netero::memck::Allocator<test>>::construct(testAllocator, object);
    object->value = 42;
    std::allocator_traits<netero::memck::Allocator<test>>::destroy(testAllocator, object);
    std::allocator_traits<netero::memck::Allocator<test>>::deallocate(testAllocator, object, 1);
    std::cout << "on_class_test -- end" << std::endl;
}

void rebind_test() {
    std::cout << "rebind_test -- start" << std::endl;
    netero::memck::Allocator<int>  allocator;
    netero::memck::Allocator<int>::rebind<char>::other  rebindAllocator;
    auto* integer = std::allocator_traits<netero::memck::Allocator<int>>::allocate(allocator, 1);
    auto* character = std::allocator_traits<netero::memck::Allocator<int>::rebind<char>::other>::allocate(rebindAllocator, 1);
    *integer = 42;
    *character = 'A';
    std::allocator_traits<netero::memck::Allocator<int>::rebind<char>::other>::deallocate(rebindAllocator, character, 1);
    std::allocator_traits<netero::memck::Allocator<int>>::deallocate(allocator, integer, 1);
    std::cout << "rebind_test -- end" << std::endl;
}

void avl_test() {
    std::cout << "avl_test -- start" << std::endl;
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
    std::cout << "avl_test -- start" << std::endl;
}

int main() {
    stl_basic_container_test();
    on_class_test();
    rebind_test();
    avl_test();
    return 0;
}

