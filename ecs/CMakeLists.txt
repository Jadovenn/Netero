cmake_minimum_required (VERSION 3.11...3.16)
project(netero_ecs
        VERSION 0.1.0
        DESCRIPTION "Netero ecs submodule"
        LANGUAGES CXX)

##====================================
##  Generic core
##====================================

list(APPEND PUBLIC_HEADER
        ${HEADERS_DIRECTORY}/ecs/world.hpp
        ${HEADERS_DIRECTORY}/ecs/entity.hpp
        ${HEADERS_DIRECTORY}/ecs/component.hpp
        ${HEADERS_DIRECTORY}/ecs/component_filter.hpp
        ${HEADERS_DIRECTORY}/ecs/system.hpp)
list(APPEND SRCS
        world.cpp
        entity.cpp
        entityContainer.cpp)

##====================================
##  Target
##====================================

add_library(netero_ecs SHARED ${SRCS} ${PUBLIC_HEADER})
add_library(netero_ecs_static STATIC ${SRCS} ${PUBLIC_HEADER})
target_compile_features(netero_ecs PUBLIC cxx_std_17)
target_compile_features(netero_ecs_static PUBLIC cxx_std_17)
target_include_directories(netero_ecs
        PUBLIC
            $<INSTALL_INTERFACE:include>
            $<BUILD_INTERFACE:${INCLUDE_DIRECTORY_PATH}>)
target_include_directories(netero_ecs_static
        PUBLIC
            $<INSTALL_INTERFACE:include>
            $<BUILD_INTERFACE:${INCLUDE_DIRECTORY_PATH}>)

##====================================
##  Installation
##====================================

install(TARGETS netero_ecs netero_ecs_static
        EXPORT ecs-export
        RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX}/lib
        LIBRARY DESTINATION ${CMAKE_INSTALL_PREFIX}/lib
        ARCHIVE DESTINATION ${CMAKE_INSTALL_PREFIX}/lib)
install(EXPORT ecs-export
        FILE
            ecsTargets.cmake
        NAMESPACE
            Netero::
        DESTINATION
            ${CMAKE_INSTALL_PREFIX}/lib/cmake/netero)
install(DIRECTORY ${INCLUDE_DIRECTORY_PATH}/netero/ecs
        DESTINATION ${CMAKE_INSTALL_PREFIX}/include/netero
        FILES_MATCHING PATTERN "*.hpp")