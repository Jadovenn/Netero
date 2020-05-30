cmake_minimum_required (VERSION 3.11...3.16)
project(neteroExtra
        VERSION 0.1.0
        DESCRIPTION "Netero extra submodule"
        LANGUAGES C CXX)

message(STATUS "Configure Netero extra submodule")

##====================================
##  Generic extra
##====================================

list(APPEND PUBLIC_HEADER
        ${HEADERS_DIRECTORY}/extra/stb_image.h
        ${HEADERS_DIRECTORY}/extra/stb_image_resize.h
        ${HEADERS_DIRECTORY}/extra/stb_image_write.h)
list(APPEND SRCS
        stb_image.c
        stb_image_resize.c
        stb_image_write.c)

##====================================
##  Target
##====================================

add_library(neteroExtra STATIC ${SRCS} ${PUBLIC_HEADER})
target_compile_features(neteroExtra PUBLIC cxx_std_17)
target_include_directories(neteroExtra
        PUBLIC
            $<INSTALL_INTERFACE:include>
            $<BUILD_INTERFACE:${INCLUDE_DIRECTORY_PATH}>)

if(WIN32 AND WIN32_STATIC)
    set_property(TARGET neteroExtra PROPERTY
        MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Release>:Release>")
endif()

##====================================
##  Installation
##====================================

install(TARGETS neteroExtra
        EXPORT extra-export
        RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX}/lib
        LIBRARY DESTINATION ${CMAKE_INSTALL_PREFIX}/lib
        ARCHIVE DESTINATION ${CMAKE_INSTALL_PREFIX}/lib)
install(EXPORT extra-export
        FILE
            extraTargets.cmake
        NAMESPACE
            Netero::
        DESTINATION
            ${CMAKE_INSTALL_PREFIX}/lib/cmake/netero)
install(DIRECTORY ${INCLUDE_DIRECTORY_PATH}/netero/extra
        DESTINATION ${CMAKE_INSTALL_PREFIX}/include/netero
        FILES_MATCHING PATTERN "*.hpp")
install(DIRECTORY ${INCLUDE_DIRECTORY_PATH}/netero/extra
        DESTINATION ${CMAKE_INSTALL_PREFIX}/include/netero
        FILES_MATCHING PATTERN "*.h")