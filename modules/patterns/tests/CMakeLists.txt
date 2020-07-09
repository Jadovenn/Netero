cmake_minimum_required (VERSION 3.11...3.16)
project(NeteroTests
        VERSION 1.0
        DESCRIPTION "Netero patterns unit test."
        LANGUAGES CXX)

add_unit_test(NAME slot_ref_test SOURCES test_slot_ref.cpp INCLUDE_DIRS ${Netero_INCLUDE_DIRS} DEPENDS Netero::Patterns)
add_unit_test(NAME slot_container_test SOURCES test_slot_container.cpp INCLUDE_DIRS ${Netero_INCLUDE_DIRS} DEPENDS Netero::Patterns)
add_unit_test(NAME signal_container_test SOURCES test_signal_container.cpp INCLUDE_DIRS ${Netero_INCLUDE_DIRS} DEPENDS Netero::Patterns)

add_unit_test(NAME ecs_world_test SOURCES ecs/test_ecs_world.cpp INCLUDE_DIRS ${Netero_INCLUDE_DIRS} DEPENDS Netero::Patterns)
add_unit_test(NAME ecs_entity_test SOURCES ecs/test_ecs_entity.cpp INCLUDE_DIRS ${Netero_INCLUDE_DIRS} DEPENDS Netero::Patterns)
add_unit_test(NAME ecs_component_test SOURCES ecs/test_ecs_component.cpp INCLUDE_DIRS ${Netero_INCLUDE_DIRS} DEPENDS Netero::Patterns)
add_unit_test(NAME ecs_component_filter_test SOURCES ecs/test_ecs_component_filter.cpp INCLUDE_DIRS ${Netero_INCLUDE_DIRS} DEPENDS Netero::Patterns)
add_unit_test(NAME ecs_system SOURCES ecs/test_ecs_system.cpp ecs/test_ecs_dataset.hpp INCLUDE_DIRS ${Netero_INCLUDE_DIRS} DEPENDS Netero::Patterns)
