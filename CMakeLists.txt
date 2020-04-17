cmake_minimum_required (VERSION 3.11...3.16)
project(netero
	VERSION 1.0
	DESCRIPTION "Real time utility library"
	LANGUAGES CXX)

##====================================
##  Init Project if not a dependency
##====================================

if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
    set(CMAKE_CXX_EXTENTIONS OFF)
    set_property(GLOBAL PROPERTY USE_FOLDERS ON)
    ##find_package(Doxygen)
    include(CTest)
    enable_testing()
    if(Doxygen_FOUND)
        add_subdirectory(docs)
    else()
        message(STATUS "Doxygen not found, not building docs")
    endif(Doxygen_FOUND)
    source_group(TREE "${PROJECT_SOURCE_DIR}/include" PREFIX "Header Files" FILES ${PUBLIC_HEADER})
endif()

# FetchContent added in CMake 3.11, downloads during the configure step
include(FetchContent)
if(${CMAKE_VERSION} VERSION_LESS 3.14)
    include(cmake/add_FetchContent_MakeAvailable.cmake)
endif()

##====================================
##  Modules declaration
##====================================

## Submodule config
OPTION(NETERO_ECS "ecs, contain Entity Component System architecture" ON)
OPTION(NETERO_AUDIO "Audio engine" ON)
OPTION(NETERO_GRAPHICS "Graphics component based on ecs submodule" ON)

## Other config
OPTION(NETERO_UNIT_TEST "Netero unit tests." OFF)
OPTION(NETERO_TOOLS "Netero additional tools." OFF)

## Build config
OPTION(WIN32_STATIC "Link staticaly with win32's dll" OFF)
OPTION(CODE_COVERAGE "Enable coverage reporting" OFF)
OPTION(MOCK_INTERFACES "Enable mock interfaces" OFF)

set(INCLUDE_DIRECTORY_PATH ${CMAKE_CURRENT_SOURCE_DIR}/include)
set(HEADERS_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/include/netero)
set(netero_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include/netero)

if(CODE_COVERAGE AND CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    # Add required flags (GCC & LLVM/Clang)
    add_library(coverage_config INTERFACE)
    target_compile_options(coverage_config INTERFACE
            -O0        # no optimization
            -g         # generate debug info
            --coverage # sets all required flags
            )
    if(CMAKE_VERSION VERSION_GREATER_EQUAL 3.13)
        target_link_options(coverage_config INTERFACE --coverage)
    else()
        target_link_libraries(coverage_config INTERFACE --coverage)
    endif()
endif()

##====================================
##  Sub-project
##====================================

add_subdirectory(modules/core)
if (NETERO_AUDIO)
    add_subdirectory(modules/audio)
endif(NETERO_AUDIO)
if (NETERO_ECS)
    add_subdirectory(modules/ecs)
endif(NETERO_ECS)
if(NETERO_GRAPHICS)
    if(NOT NETERO_ECS)
        message(FATAL_ERROR "GRAPHICS rely on ECS, ECS must be activated")
    endif(NOT NETERO_ECS)
    add_subdirectory(modules/graphics)
endif(NETERO_GRAPHICS)

if(NETERO_TOOLS)
    add_subdirectory(tools)
endif(NETERO_TOOLS)

##====================================
##  Global install rules
##====================================

install(FILES cmake/netero-config.cmake
        DESTINATION ${CMAKE_INSTALL_PREFIX}/lib)

##====================================
##  Tests
##====================================

if((CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME OR MODERN_CMAKE_BUILD_TESTING) AND BUILD_TESTING AND NETERO_UNIT_TEST)
    add_subdirectory(tests)
endif()

