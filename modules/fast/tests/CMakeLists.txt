cmake_minimum_required (VERSION 3.11...3.16)
project(NeteroTests
        VERSION 1.0
        DESCRIPTION "Netero patterns unit test."
        LANGUAGES CXX)

add_unit_test(NAME Fast_easing_test
                SOURCES
                    easing_test.cpp
                    low_pass_filter.cpp
                INCLUDE_DIRS
                    ${Netero_INCLUDE_DIRS}
                DEPENDS
                    gtest_main
                    Netero::Netero
                    Netero::Fast)
