cmake_minimum_required(VERSION 3.11...3.16)
project(netero
        VERSION 1.0
        DESCRIPTION "Real time utility library"
        LANGUAGES CXX)

##====================================
##  Init Project if not a dependency
##====================================

if (CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
    set(CMAKE_CXX_EXTENTIONS OFF)
    set_property(GLOBAL PROPERTY USE_FOLDERS ON)
    include(CTest)
    include(CPack)
    include(cmake/add_test_project.cmake)
    include(FetchContent)
    if (${CMAKE_VERSION} VERSION_LESS 3.14)
        include(cmake/add_FetchContent_MakeAvailable.cmake)
    endif ()
    if (NETERO_UNIT_TEST)
        enable_testing()
        message("-- Cloning GTest")
        FetchContent_Declare(
                GTest
                GIT_REPOSITORY https://github.com/google/googletest.git
        )
        FetchContent_MakeAvailable(GTest)
    endif (NETERO_UNIT_TEST)
    source_group(TREE "${PROJECT_SOURCE_DIR}/include" PREFIX "Header Files" FILES ${PUBLIC_HEADER})
endif ()

add_compile_options("$<$<CONFIG:RELEASE>:-O2>")

##====================================
##  Modules declaration
##====================================

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

## Submodule config
OPTION(NETERO_FAST "Fast module library." ON)
OPTION(NETERO_AUDIO "Audio module library." ON)
OPTION(NETERO_GRAPHICS "Graphics module library." ON)
OPTION(NETERO_PATTERNS "Patterns module library." ON)
OPTION(NETERO_EXTRA "Self hosted STB lib components." ON)
OPTION(NETERO_MEMCHECK "Memory checker library." ON)
OPTION(NETERO_CONSOLE "Console helper library." ON)

## Other config
OPTION(NETERO_UNIT_TEST "Netero unit tests." OFF)
OPTION(NETERO_SAMPLES "Netero samples." OFF)

## Build config mainly for CI
OPTION(WIN32_STATIC "Link statically with win32's dll" OFF)
OPTION(CODE_COVERAGE "Enable coverage reporting" OFF)
OPTION(MOCK_INTERFACES "Enable mock interfaces" OFF)

if (CODE_COVERAGE AND CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    # Add required flags (GCC & LLVM/Clang)
    add_library(coverage_config INTERFACE)
    target_compile_options(coverage_config INTERFACE
            -O0
            -g
            --coverage
            )
    if (CMAKE_VERSION VERSION_GREATER_EQUAL 3.13)
        target_link_options(coverage_config INTERFACE --coverage)
    else ()
        target_link_libraries(coverage_config INTERFACE --coverage)
    endif ()
endif ()

##====================================
##  Extra CMAKE script by modules
##====================================

if (NETERO_GRAPHICS)
    include(cmake/add_graphics_function.cmake)
endif (NETERO_GRAPHICS)

if (NETERO_FAST)
    include(cmake/enable_nasm.cmake)
endif (NETERO_FAST)

##====================================
##  include modules projects
##====================================

add_subdirectory(modules/Core)

if (NETERO_FAST)
    add_subdirectory(modules/Fast)
endif (NETERO_FAST)

if (NETERO_PATTERNS)
    add_subdirectory(modules/patterns)
endif (NETERO_PATTERNS)

if (NETERO_EXTRA)
    add_subdirectory(modules/extra)
endif (NETERO_EXTRA)

if (NETERO_AUDIO)
    add_subdirectory(modules/audio)
endif (NETERO_AUDIO)

if (NETERO_CONSOLE)
    add_subdirectory(modules/console)
endif (NETERO_CONSOLE)

if (NETERO_GRAPHICS)
    if (NOT NETERO_EXTRA OR NOT NETERO_CONSOLE)
        message(FATAL_ERROR "GRAPHICS rely on EXTRA and CONSOLE")
    endif (NOT NETERO_EXTRA OR NOT NETERO_CONSOLE)
    add_subdirectory(modules/graphics)
endif (NETERO_GRAPHICS)

if (NETERO_MEMCHECK)
    add_subdirectory(modules/memcheck)
endif (NETERO_MEMCHECK)

if (NETERO_SAMPLES)
    add_subdirectory(samples)
endif (NETERO_SAMPLES)

##====================================
##  Global install rules
##====================================

install(FILES cmake/Netero-config.cmake
        DESTINATION ${CMAKE_INSTALL_PREFIX}/cmake)
