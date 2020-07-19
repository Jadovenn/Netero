cmake_minimum_required(VERSION 3.11...3.16)
project(neteroFast
        VERSION 0.1.0
        DESCRIPTION "Netero fast module"
        LANGUAGES CXX)

message(STATUS "Configure Netero Fast module.")

##====================================
##  Sources
##====================================

set(FAST_PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/private)
set(FAST_PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/public)

list(APPEND PUBLIC_HEADER
        public/netero/fast/simd.hpp
        public/netero/fast/easing.hpp)
list(APPEND SRCS
        private/easing/quadratic_easing.cpp)

##====================================
##  Target
##====================================

add_library(NeteroFast STATIC ${SRCS} ${PUBLIC_HEADER})
##add_dependencies(NeteroFast Netero::Netero)
target_compile_features(NeteroFast PUBLIC cxx_std_17)
target_include_directories(NeteroFast
        PUBLIC
        $<INSTALL_INTERFACE:include>
        $<BUILD_INTERFACE:${FAST_PUBLIC}>
        PRIVATE
        $<BUILD_INTERFACE:${Netero_INCLUDE_DIRS}>)

if (WIN32 AND WIN32_STATIC)
    set_property(TARGET NeteroFast PROPERTY
            MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Release>:Release>")
endif ()

add_library(Netero::Fast ALIAS NeteroFast)

##====================================
##  Cached Variables
##====================================

set(Netero_Fast_FOUND True CACHE INTERNAL "Netero Fast FOUND flag.")

set(Netero_Fast_INCLUDE_DIRS "${FAST_PUBLIC}" CACHE INTERNAL "" FORCE)

set(Netero_INCLUDE_DIRS
        "${Netero_INCLUDE_DIRS}"
        "${FAST_PUBLIC}" CACHE INTERNAL "Netero include directories.")

##====================================
##  Installation
##====================================

install(TARGETS NeteroFast
        EXPORT NeteroFast-export
        RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX}/lib
        LIBRARY DESTINATION ${CMAKE_INSTALL_PREFIX}/lib
        ARCHIVE DESTINATION ${CMAKE_INSTALL_PREFIX}/lib)
install(EXPORT NeteroFast-export
        FILE
        NeteroFastTargets.cmake
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
    add_subdirectory(tests)
endif ()
