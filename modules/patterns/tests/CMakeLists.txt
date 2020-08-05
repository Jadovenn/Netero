cmake_minimum_required (VERSION 3.11...3.16)
project(NeteroTests
        VERSION 1.0
        DESCRIPTION "Netero patterns unit test."
        LANGUAGES CXX)

add_unit_test(NAME Patterns_Signal_test
                SOURCES
                    test_slot_ref.cpp
                    test_slot_container.cpp
                    test_signal_container.cpp
                INCLUDE_DIRS
                    ${Netero_INCLUDE_DIRS}
                DEPENDS
                    gtest_main
                    Netero::Netero
                    Netero::Patterns)

add_unit_test(NAME Patterns_ECS_test
                SOURCES
                    ecs/test_ecs_world.cpp
                    ecs/test_ecs_entity.cpp
                    ecs/test_ecs_component.cpp
                    ecs/test_ecs_component_filter.cpp
                    ecs/test_ecs_system.cpp
                    ecs/test_ecs_dataset.hpp
                INCLUDE_DIRS
                    ${Netero_INCLUDE_DIRS}
                DEPENDS
                    gtest_main
                    Netero::Netero
                    Netero::Patterns)

