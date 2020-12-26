cmake_minimum_required(VERSION 3.11...3.16)
project(NeteroTests
        VERSION 1.0
        DESCRIPTION "Netero unit test."
        LANGUAGES CXX)

add_unit_test(NAME Core_Algo_test
        SOURCES
        avl_test.cpp
        set_test.cpp
        buffer_test.cpp
        size_buffer_bug_test.cpp
        type_id_test.cpp
        RandomEngine_test.cpp
        INCLUDE_DIRS
        ${Netero_INCLUDE_DIRS}
        DEPENDS
        gtest_main
        Netero::Netero)
