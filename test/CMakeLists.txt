project(Test)

set(NETERO_LINK_LIBRARY ntlibs)

add_executable(test_set test_set.cpp)
target_include_directories(test_set PUBLIC NETERO_INCLUDE_DIR)
target_link_libraries(test_set ${NETERO_LINK_LIBRARY})
add_test(NAME test_set COMMAND ./test_set)
set_target_properties(test_set
        PROPERTIES
        LINKER_LANGUAGE CXX
        CXX_STANDARD 17)

add_executable(test_avl test_avl.cpp)
target_include_directories(test_avl PUBLIC NETERO_INCLUDE_DIR)
target_link_libraries(test_avl ${NETERO_LINK_LIBRARY})
add_test(NAME test_avl COMMAND ./test_avl)
set_target_properties(test_avl
        PROPERTIES
        LINKER_LANGUAGE CXX
        CXX_STANDARD 17)

# core Test
add_executable(test_core_type_id core/test_core_type_id.cpp)
target_include_directories(test_core_type_id PUBLIC NETERO_INCLUDE_DIR)
target_link_libraries(test_core_type_id ${NETERO_LINK_LIBRARY})
add_test(NAME test_core_type_id COMMAND ./test_core_type_id)
set_target_properties(test_core_type_id
        PROPERTIES
        LINKER_LANGUAGE CXX
        CXX_STANDARD 17)

# ECS Test
add_executable(test_ecs_world ecs/test_ecs_world.cpp)
target_include_directories(test_ecs_world PUBLIC NETERO_INCLUDE_DIR)
target_link_libraries(test_ecs_world ${NETERO_LINK_LIBRARY})
add_test(NAME test_ecs_world COMMAND ./test_ecs_world)
set_target_properties(test_ecs_world
        PROPERTIES
        LINKER_LANGUAGE CXX
        CXX_STANDARD 17)

add_executable(test_ecs_entity ecs/test_ecs_entity.cpp)
target_include_directories(test_ecs_entity PUBLIC NETERO_INCLUDE_DIR)
target_link_libraries(test_ecs_entity ${NETERO_LINK_LIBRARY})
add_test(NAME test_ecs_entity COMMAND ./test_ecs_entity)
set_target_properties(test_ecs_entity
        PROPERTIES
        LINKER_LANGUAGE CXX
        CXX_STANDARD 17)

add_executable(test_ecs_component ecs/test_ecs_component.cpp)
target_include_directories(test_ecs_component PUBLIC NETERO_INCLUDE_DIR)
target_link_libraries(test_ecs_component ${NETERO_LINK_LIBRARY})
add_test(NAME test_ecs_component COMMAND ./test_ecs_component)
set_target_properties(test_ecs_component
        PROPERTIES
        LINKER_LANGUAGE CXX
        CXX_STANDARD 17)

add_executable(test_ecs_component_filter ecs/test_ecs_component_filter.cpp)
target_include_directories(test_ecs_component_filter PUBLIC NETERO_INCLUDE_DIR)
target_link_libraries(test_ecs_component_filter ${NETERO_LINK_LIBRARY})
add_test(NAME test_ecs_component_filter COMMAND ./test_ecs_component)
set_target_properties(test_ecs_component_filter
        PROPERTIES
        LINKER_LANGUAGE CXX
        CXX_STANDARD 17)

add_executable(test_ecs_system ecs/test_ecs_system.cpp ecs/test_ecs_dataset.hpp)
target_include_directories(test_ecs_system PUBLIC NETERO_INCLUDE_DIR)
target_link_libraries(test_ecs_system ${NETERO_LINK_LIBRARY})
add_test(NAME test_ecs_system COMMAND ./test_ecs_system)
set_target_properties(test_ecs_system
        PROPERTIES
        LINKER_LANGUAGE CXX
        CXX_STANDARD 17)
