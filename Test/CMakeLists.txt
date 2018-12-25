project(Test)

add_executable(ecs_world ECS/ecs_world.cpp)
add_test(NAME ecs_world COMMAND ./ecs_world)
