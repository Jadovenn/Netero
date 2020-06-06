
#include <iostream>
#include <vector>
#include <list>
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

int main() {
    stl_basic_container_test();
    on_class_test();
    return 0;
}

