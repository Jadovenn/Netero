project(Test)

# Core Test
add_executable(core_type_id Core/core_type_id.cpp)
target_include_directories(core_type_id PUBLIC NETERO_INCLUDE_DIR)
target_link_libraries(core_type_id Netero)
add_test(NAME core_type_id COMMAND ./core_type_id)

# ECS Test
add_executable(ecs_world ECS/ecs_world.cpp)
target_include_directories(ecs_world PUBLIC NETERO_INCLUDE_DIR)
target_link_libraries(ecs_world Netero)
add_test(NAME ecs_world COMMAND ./ecs_world)

add_executable(ecs_entity ECS/ecs_entity.cpp)
target_include_directories(ecs_entity PUBLIC NETERO_INCLUDE_DIR)
target_link_libraries(ecs_entity Netero)
add_test(NAME ecs_entity COMMAND ./ecs_entity)

add_executable(ecs_component ECS/ecs_component.cpp)
target_include_directories(ecs_component PUBLIC NETERO_INCLUDE_DIR)
target_link_libraries(ecs_component Netero)
add_test(NAME ecs_component COMMAND ./ecs_component)

add_executable(ecs_component_filter ECS/ecs_component_filter.cpp)
target_include_directories(ecs_component_filter PUBLIC NETERO_INCLUDE_DIR)
target_link_libraries(ecs_component_filter Netero)
add_test(NAME ecs_component_filter COMMAND ./ecs_component)

add_executable(ecs_system ECS/ecs_system.cpp ECS/ecs_dataset.hpp)
target_include_directories(ecs_system PUBLIC NETERO_INCLUDE_DIR)
target_link_libraries(ecs_system Netero)
add_test(NAME ecs_system COMMAND ./ecs_system)
