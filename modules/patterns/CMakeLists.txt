cmake_minimum_required (VERSION 3.11...3.16)
project(neteroECS
        VERSION 0.1.0
        DESCRIPTION "Netero patterns module"
        LANGUAGES CXX)

message(STATUS "Configure Netero Patterns module.")

##====================================
##  Sources
##====================================

set(PATTERNS_PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/private)
set(PATTERNS_PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/public)

list(APPEND PUBLIC_HEADER
        public/netero/ecs/world.hpp
        public/netero/ecs/entity.hpp
        public/netero/ecs/component.hpp
        public/netero/ecs/component_filter.hpp
        public/netero/ecs/system.hpp
        public/netero/patterns/IObserver.hpp
	public/netero/patterns/IFactory.hpp
        public/netero/signal.hpp
        public/netero/slot.hpp)
list(APPEND SRCS
        private/ecs/world.cpp
        private/ecs/entity.cpp
        private/ecs/entityContainer.cpp)

##====================================
##  Target
##====================================

add_library(NeteroPatterns STATIC ${SRCS} ${PUBLIC_HEADER})
add_dependencies(NeteroPatterns Netero)
target_compile_features(NeteroPatterns PUBLIC cxx_std_17)
target_include_directories(NeteroPatterns
        PUBLIC
            $<INSTALL_INTERFACE:include>
            $<BUILD_INTERFACE:${PATTERNS_PUBLIC}>
        PRIVATE
            $<BUILD_INTERFACE:${Netero_INCLUDE_DIRS}>)

if(WIN32 AND WIN32_STATIC)
    set_property(TARGET NeteroPatterns PROPERTY
        MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Release>:Release>")
endif()

add_library(Netero::Patterns ALIAS NeteroPatterns)

##====================================
##  Cached Variables
##====================================

set(Netero_Patterns_FOUND True CACHE INTERNAL "Netero Patterns FOUND flag.")

set(Netero_INCLUDE_DIRS
        ${Netero_INCLUDE_DIRS}
        ${PATTERNS_PUBLIC} CACHE INTERNAL "Netero include directories.")

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

if(BUILD_TESTING AND NETERO_UNIT_TEST)
    add_subdirectory(tests)
endif()

