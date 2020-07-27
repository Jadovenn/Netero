cmake_minimum_required (VERSION 3.11...3.16)
project(netero_basics
    VERSION 1.0
    DESCRIPTION "Netero basics samples programmes."
    LANGUAGES CXX)

add_executable(os_sample os_sample.cpp)
add_dependencies(os_sample Netero::Netero)
target_include_directories(os_sample PUBLIC ${Netero_INCLUDE_DIRS})
target_link_libraries(os_sample Netero::Netero)

