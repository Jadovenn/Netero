cmake_minimum_required (VERSION 3.11...3.16)
project(NeteroTests
        VERSION 1.0
        DESCRIPTION "Netero Console unit test."
        LANGUAGES CXX)

add_unit_test(NAME Console_unit_test
        SOURCES
            console_test.cpp
        INCLUDE_DIRS
            ${Netero_INCLUDE_DIRS}
        DEPENDS
            gtest_main
            Netero::Netero
            Netero::Console)
