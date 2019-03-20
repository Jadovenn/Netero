project(Test)



# core Test
add_executable(core_type_id core/core_type_id.cpp)
target_include_directories(core_type_id PUBLIC NETERO_INCLUDE_DIR)
target_link_libraries(core_type_id Netero)
add_test(NAME core_type_id COMMAND ./core_type_id)
set_target_properties(core_type_id
        PROPERTIES
        LINKER_LANGUAGE CXX
        CXX_STANDARD 17)

# ECS Test
add_executable(ecs_world ecs/ecs_world.cpp)
target_include_directories(ecs_world PUBLIC NETERO_INCLUDE_DIR)
target_link_libraries(ecs_world Netero)
add_test(NAME ecs_world COMMAND ./ecs_world)
set_target_properties(ecs_world
        PROPERTIES
        LINKER_LANGUAGE CXX
        CXX_STANDARD 17)

add_executable(ecs_entity ecs/ecs_entity.cpp)
target_include_directories(ecs_entity PUBLIC NETERO_INCLUDE_DIR)
target_link_libraries(ecs_entity Netero)
add_test(NAME ecs_entity COMMAND ./ecs_entity)
set_target_properties(ecs_entity
        PROPERTIES
        LINKER_LANGUAGE CXX
        CXX_STANDARD 17)

add_executable(ecs_component ecs/ecs_component.cpp)
target_include_directories(ecs_component PUBLIC NETERO_INCLUDE_DIR)
target_link_libraries(ecs_component Netero)
add_test(NAME ecs_component COMMAND ./ecs_component)
set_target_properties(ecs_component
        PROPERTIES
        LINKER_LANGUAGE CXX
        CXX_STANDARD 17)

add_executable(ecs_component_filter ecs/ecs_component_filter.cpp)
target_include_directories(ecs_component_filter PUBLIC NETERO_INCLUDE_DIR)
target_link_libraries(ecs_component_filter Netero)
add_test(NAME ecs_component_filter COMMAND ./ecs_component)
set_target_properties(ecs_component_filter
        PROPERTIES
        LINKER_LANGUAGE CXX
        CXX_STANDARD 17)

add_executable(ecs_system ecs/ecs_system.cpp ECS/ecs_dataset.hpp)
target_include_directories(ecs_system PUBLIC NETERO_INCLUDE_DIR)
target_link_libraries(ecs_system Netero)
add_test(NAME ecs_system COMMAND ./ecs_system)
set_target_properties(ecs_system
        PROPERTIES
        LINKER_LANGUAGE CXX
        CXX_STANDARD 17)
