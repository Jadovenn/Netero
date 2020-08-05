cmake_minimum_required (VERSION 3.11...3.16)
project(NeteroMemcheckTests
        VERSION 1.0
        DESCRIPTION "Netero Memcheck unit test."
        LANGUAGES CXX)

add_executable(new_sample new_sample.cpp)
add_dependencies(new_sample Netero::Netero Netero::Memcheck)
target_compile_features(new_sample PUBLIC cxx_std_17)
target_include_directories(new_sample PUBLIC ${Netero_INCLUDE_DIRS})
target_link_libraries(new_sample Netero::Netero Netero::Memcheck)

add_executable(allocator_sample allocator_sample.cpp)
add_dependencies(allocator_sample Netero::Netero Netero::Memcheck)
target_compile_features(allocator_sample PUBLIC cxx_std_17)
target_include_directories(allocator_sample PUBLIC ${Netero_INCLUDE_DIRS})
target_link_libraries(allocator_sample Netero::Netero Netero::Memcheck)

