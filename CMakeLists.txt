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
    find_package(Doxygen)
    include(CTest)
    enable_testing()
    add_library(coverage_config INTERFACE)
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

OPTION(ECS "ecs, contain Entity Component System architecture" ON)
OPTION(AUDIO "Audio engine" ON)
OPTION(ENGINE "Rendering engine based on vulkan" OFF)
option(CODE_COVERAGE "Enable coverage reporting" ON)

set(INCLUDE_DIRECTORY_PATH ${CMAKE_CURRENT_SOURCE_DIR}/include)
set(HEADERS_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/include/netero)

if(CODE_COVERAGE AND CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    # Add required flags (GCC & LLVM/Clang)
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

add_subdirectory(core)
if (AUDIO)
    add_subdirectory(audio)
endif(AUDIO)
if (ECS)
    add_subdirectory(ecs)
endif(ECS)
if(ENGINE)
    if(NOT ECS)
        message(FATAL_ERROR "ENGINE rely on ECS, ECS must be activated")
    endif(NOT ECS)
    add_subdirectory(engine)
endif(ENGINE)

##====================================
##  Global install rules
##====================================

install(FILES cmake/netero-config.cmake
        DESTINATION ${CMAKE_INSTALL_PREFIX}/lib)

##====================================
##  Tests
##====================================

if((CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME OR MODERN_CMAKE_BUILD_TESTING) AND BUILD_TESTING)
    add_subdirectory(tests)
endif()
