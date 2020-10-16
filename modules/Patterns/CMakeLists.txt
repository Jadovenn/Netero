cmake_minimum_required(VERSION 3.11...3.16)
project(neteroPattern
        VERSION 0.1.0
        DESCRIPTION "Netero patterns module"
        LANGUAGES CXX)

message(STATUS "Configure Netero Patterns module.")

##====================================
##  Sources
##====================================

set(PATTERNS_PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/Private)
set(PATTERNS_PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/Public)

list(APPEND PUBLIC_HEADER
        Public/Netero/ECS/World.hpp
        Public/Netero/ECS/Entity.hpp
        Public/Netero/ECS/Component.hpp
        Public/Netero/ECS/ComponentFilter.hpp
        Public/Netero/ECS/System.hpp
        public/Netero/patterns/IObserver.hpp
        public/Netero/patterns/IFactory.hpp
        public/Netero/patterns/ISingleton.hpp
        Public/Netero/Signal.hpp
        Public/Netero/Slot.hpp)
list(APPEND SRCS
        Private/ECS/World.cpp
        Private/ECS/Entity.cpp
        Private/ECS/EntityContainer.cpp)

##====================================
##  Target
##====================================

add_library(NeteroPatterns STATIC ${SRCS} ${PUBLIC_HEADER})
add_dependencies(NeteroPatterns Netero::Netero)
target_compile_features(NeteroPatterns PUBLIC cxx_std_17)
target_include_directories(NeteroPatterns
        PUBLIC
        $<INSTALL_INTERFACE:include>
        $<BUILD_INTERFACE:${PATTERNS_PUBLIC}>
        PRIVATE
        $<BUILD_INTERFACE:${Netero_Core_INCLUDE_DIRS}>)

if (WIN32 AND WIN32_STATIC)
    set_property(TARGET NeteroPatterns PROPERTY
            MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Release>:Release>")
endif ()

add_library(Netero::Patterns ALIAS NeteroPatterns)

##====================================
##  Cached Variables
##====================================

set(Netero_Patterns_FOUND True CACHE INTERNAL "Netero Patterns FOUND flag.")

set(Netero_Patterns_INCLUDE_DIRS "${PATTERNS_PUBLIC}" CACHE INTERNAL "" FORCE)

set(Netero_INCLUDE_DIRS
        "${Netero_INCLUDE_DIRS}"
        "${PATTERNS_PUBLIC}" CACHE INTERNAL "Netero include directories.")

##====================================
##  Installation
##====================================

install(TARGETS NeteroPatterns
        EXPORT NeteroPatterns-export
        RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX}/lib
        LIBRARY DESTINATION ${CMAKE_INSTALL_PREFIX}/lib
        ARCHIVE DESTINATION ${CMAKE_INSTALL_PREFIX}/lib)
install(EXPORT NeteroPatterns-export
        FILE
        NeteroPatternsTargets.cmake
        NAMESPACE
        Netero::
        DESTINATION
        ${CMAKE_INSTALL_PREFIX}/cmake/netero)
install(DIRECTORY public/netero
        DESTINATION ${CMAKE_INSTALL_PREFIX}/include
        FILES_MATCHING PATTERN "*.hpp")

##====================================
##  Tests
##====================================

if (BUILD_TESTING AND NETERO_UNIT_TEST)
    add_subdirectory(Tests)
endif ()

