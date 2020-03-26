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
	include(CTest)
	enable_testing()

    find_package(Doxygen)
    if(Doxygen_FOUND)
		add_subdirectory(docs)
    else()
		message(STATUS "Doxygen not found, not building docs")
    endif()
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
OPTION(ENGINE "Engine, is a 3D engine using vulkan as backend" ON)
OPTION(POOL "Pool, async containers" ON)
OPTION(OBSERVER "Observer, contain a Qt signals/slots like, implementation" ON)
OPTION(AUDIO "Audio engine" ON)

set(INCLUDE_DIRECTORY_PATH ${CMAKE_CURRENT_SOURCE_DIR}/include)
set(HEADERS_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/include/netero)

##====================================
##  Sub-project
##====================================

add_subdirectory(core)
if (AUDIO)
    add_subdirectory(audio)
endif()
if (ECS)
    add_subdirectory(ecs)
endif()

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
