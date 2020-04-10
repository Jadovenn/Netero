cmake_minimum_required (VERSION 3.11...3.16)
project(netero_graphics
        VERSION 0.1.0
        DESCRIPTION "Netero graphics submodule"
        LANGUAGES CXX)

message(STATUS "Configure Netero graphics submodule")

##====================================
##  Dependency
##====================================

FetchContent_Declare(
        glfw
        GIT_REPOSITORY https://github.com/glfw/glfw.git
)
FetchContent_MakeAvailable(glfw)
find_package(Vulkan)

if(NOT Vulkan_FOUND)
    message(FATAL_ERROR "Please install vulkan SDK.")
endif()

##====================================
##  Generic core
##====================================

list(APPEND PUBLIC_HEADER
        )
list(APPEND SRCS
        backend/backend.hpp
        backend/backend.cpp)

##====================================
##  Target
##====================================

add_library(netero_graphics STATIC ${SRCS} ${PUBLIC_HEADER})
target_compile_features(netero_graphics PUBLIC cxx_std_17)
target_include_directories(netero_graphics
        PUBLIC
        $<INSTALL_INTERFACE:include>
        $<BUILD_INTERFACE:${INCLUDE_DIRECTORY_PATH}>
        $<BUILD_INTERFACE:${Vulkan_INCLUDE_DIRS}>
        $<BUILD_INTERFACE:${glfw_INCLUDE_DIRS}>)
target_link_libraries(netero_graphics
        PRIVATE
            Vulkan::Vulkan glfw netero_ecs)

##====================================
##  Installation
##====================================

install(TARGETS netero_graphics
        EXPORT engine-export
        RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX}/lib
        LIBRARY DESTINATION ${CMAKE_INSTALL_PREFIX}/lib
        ARCHIVE DESTINATION ${CMAKE_INSTALL_PREFIX}/lib)
install(EXPORT graphics-export
        FILE
            graphicsTargets.cmake
        NAMESPACE
            Netero::
        DESTINATION
            ${CMAKE_INSTALL_PREFIX}/lib/cmake/netero)
install(DIRECTORY ${INCLUDE_DIRECTORY_PATH}/netero/graphics
        DESTINATION ${CMAKE_INSTALL_PREFIX}/include/netero
        FILES_MATCHING PATTERN "*.hpp")

