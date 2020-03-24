cmake_minimum_required (VERSION 3.11...3.16)
project(netero
	VERSION 1.0
	DESCRIPTION "Real time utility library"
	LANGUAGES CXX)

# Only if this is the main project, not included threw add_subdirectory
if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)

	set(CMAKE_CXX_EXTENTIONS OFF)
	set_property(GLOBAL PROPERTY USE_FOLDERS ON)
	include(CTest)
	enable_testing()

    find_package(Doxygen)
    if(Doxygen_FOUND)
		add_subdirectory(docs)
    else()
		message(STATUS "Doxygen not found, not building docs")
    endif()

endif()

# FetchContent added in CMake 3.11, downloads during the configure step
include(FetchContent)
# FetchContent_MakeAvailable was not added until CMake 3.14
if(${CMAKE_VERSION} VERSION_LESS 3.14)
    include(cmake/add_FetchContent_MakeAvailable.cmake)
endif()

# Modules declaration
OPTION(ECS "ecs, contain Entity Component System architecture" ON)
OPTION(ENGINE "Engine, is a 3D engine using vulkan as backend" OFF)
OPTION(POOL "Pool, async containers" ON)
OPTION(OBSERVER "Observer, contain a Qt signals/slots like, implementation" ON)
OPTION(AUDIO "Audio engine" ON)

add_subdirectory(sources)

if((CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME OR MODERN_CMAKE_BUILD_TESTING) AND BUILD_TESTING)
    add_subdirectory(tests)
endif()
