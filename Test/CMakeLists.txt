project(Test)

add_executable(ecs_world ECS/ecs_world.cpp)
target_include_directories(ecs_world PUBLIC NETERO_INCLUDE_DIR)
target_link_libraries(ecs_world Netero)
add_test(NAME ecs_world COMMAND ./ecs_world)
