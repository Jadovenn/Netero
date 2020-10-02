cmake_minimum_required(VERSION 3.11...3.16)
project(SpvToH
        VERSION 0.1.0
        DESCRIPTION "Netero SpvToH tool"
        LANGUAGES CXX)

message(STATUS "Configure Netero SpvToH tool.")

##====================================
##  Sources
##====================================

set(SRCS
        Private/SpvToH.hpp
        Private/SpvToH.cpp
        Private/SpvToHUtility.cpp)

##====================================
##  Target
##====================================

add_executable(SpvToH ${SRCS})
add_dependencies(SpvToH Netero NeteroConsole)
target_compile_features(SpvToH PUBLIC cxx_std_17)
target_include_directories(SpvToH
        PUBLIC
            $<BUILD_INTERFACE:${Netero_INCLUDE_DIRS}>)
target_link_libraries(SpvToH
        PUBLIC
            Netero
            NeteroConsole)
