cmake_minimum_required(VERSION 3.11...3.16)
project(neteroExtra
        VERSION 0.1.0
        DESCRIPTION "Netero extra submodule"
        LANGUAGES C CXX)

message(STATUS "Configure Netero extra submodule")

##====================================
##  Sources
##====================================

set(EXTRA_PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/Private)
set(EXTRA_PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/Public)

list(APPEND PUBLIC_HEADER
        Public/Netero/Extra/stb_image.h
        Public/Netero/Extra/stb_image_resize.h
        Public/Netero/Extra/stb_image_write.h
        Public/Netero/Extra/tiny_obj_loader.hpp)
list(APPEND SRCS
        Private/stb_image.c
        Private/stb_image_resize.c
        Private/stb_image_write.c
        Private/tiny_obj_loader.cpp)

##====================================
##  Target
##====================================

add_library(NeteroExtra STATIC ${SRCS} ${PUBLIC_HEADER})
target_compile_features(NeteroExtra PUBLIC cxx_std_17)
target_include_directories(NeteroExtra
        PUBLIC
        $<INSTALL_INTERFACE:include>
        $<BUILD_INTERFACE:${EXTRA_PUBLIC}>)

if (WIN32 AND WIN32_STATIC)
    set_property(TARGET NeteroExtra PROPERTY
            MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Release>:Release>")
endif ()

add_library(Netero::Extra ALIAS NeteroExtra)

##====================================
##  Cached Variables
##====================================

set(Netero_Extra_FOUND True CACHE INTERNAL "Netero Extra FOUND flag.")

set(Netero_Extra_INCLUDE_DIRS "${EXTRA_PUBLIC}" CACHE INTERNAL "" FORCE)

set(Netero_INCLUDE_DIRS
        "${Netero_INCLUDE_DIRS}"
        "${EXTRA_PUBLIC}" CACHE INTERNAL "Netero include directories.")

##====================================
##  Installation
##====================================

install(TARGETS NeteroExtra
        EXPORT NeteroExtra-export
        RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX}/lib
        LIBRARY DESTINATION ${CMAKE_INSTALL_PREFIX}/lib
        ARCHIVE DESTINATION ${CMAKE_INSTALL_PREFIX}/lib)
install(EXPORT NeteroExtra-export
        FILE
        NeteroExtraTargets.cmake
        NAMESPACE
        Netero::
        DESTINATION
        ${CMAKE_INSTALL_PREFIX}/cmake/netero)
install(DIRECTORY public/netero
        DESTINATION ${CMAKE_INSTALL_PREFIX}/include
        FILES_MATCHING PATTERN "*.hpp")
install(DIRECTORY public/netero
        DESTINATION ${CMAKE_INSTALL_PREFIX}/include
        FILES_MATCHING PATTERN "*.h")