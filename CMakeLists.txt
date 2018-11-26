cmake_minimum_required(VERSION 3.8)
project(Netero VERSION 1.0.0 LANGUAGES CXX)

include(CTest)
enable_testing()

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CURRENT_BINARY_DIR Build)

add_subdirectory(Test)
