
#include <vector>
#include <list>
#include <netero/memcheck/allocator.hpp>

int main() {
    std::vector<int, netero::memck::Allocator<int>>  myVector;
    myVector.push_back(42);
    myVector.resize(100);
    myVector.shrink_to_fit();
    myVector.pop_back();
    std::list<float, netero::memck::Allocator<float>> myList;
    myList.push_back(42.f);
    myList.pop_back();
    return 0;
}

