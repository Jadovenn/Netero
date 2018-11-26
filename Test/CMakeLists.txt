project(Test)

set(first_src
        first.cpp)
add_executable(first ${first_src})

add_test(NAME first_test COMMAND ./first)
