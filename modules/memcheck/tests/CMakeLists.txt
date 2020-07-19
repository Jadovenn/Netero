cmake_minimum_required (VERSION 3.11...3.16)
project(NeteroMemcheckTests
        VERSION 1.0
        DESCRIPTION "Netero Memcheck unit test."
        LANGUAGES CXX)

add_unit_test(NAME allocator_test SOURCES test_allocator.cpp INCLUDE_DIRS ${Netero_INCLUDE_DIRS} DEPENDS Netero::Netero Netero::NeteroMemcheck)
add_unit_test(NAME new_test SOURCES test_new.cpp INCLUDE_DIRS ${Netero_INCLUDE_DIRS} DEPENDS Netero::Netero Netero::NeteroMemcheck)
