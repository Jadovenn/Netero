cmake_minimum_required(VERSION 3.11...3.16)
project(NeteroGraphics
        VERSION 0.1.0
        DESCRIPTION "Netero graphics module"
        LANGUAGES CXX)

message(STATUS "Configure Netero Graphics module.")

##====================================
##  Extern Dependencies
##====================================

set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)

find_package(Vulkan)
if (NOT Vulkan_FOUND)
    message(FATAL_ERROR "Please install vulkan SDK.")
endif ()

FetchContent_Declare(
        glfw
        GIT_REPOSITORY https://github.com/glfw/glfw.git
)
FetchContent_MakeAvailable(glfw)
FetchContent_Declare(
        glm
        GIT_REPOSITORY https://github.com/g-truc/glm.git
)
FetchContent_MakeAvailable(glm)
#FetchContent_GetProperties(glm)
#if(NOT glm_POPULATED)
#    FetchContent_Populate(glm)
#    set(GLM_TEST_ENABLE OFF CACHE BOOL "" FORCE)
#    add_subdirectory(${glm_SOURCE_DIR} ${glm_BINARY_DIR})
#endif()

##====================================
##  Sources
##====================================

set(Graphics_PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/Private)
set(Graphics_PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/Public)

list(APPEND PUBLIC_HEADER
        Public/Netero/Graphics/Application.hpp
        Public/Netero/Graphics/Window.hpp
        Public/Netero/Graphics/Renderer.hpp
        Public/Netero/Graphics/Mesh.hpp
        Public/Netero/Graphics/Surface.hpp
        Public/Netero/Graphics/Shader.hpp
        Public/Netero/Graphics/Texture.hpp)

list(APPEND SRCS
        Private/Vulkan/Vulkan.hpp
        Private/Vulkan/Context/Context.hpp
        Private/Vulkan/Context/Context.cpp
        Private/Vulkan/VkUtils.hpp
        Private/Vulkan/Utils/VkValidationLayerSupport.cpp
        Private/Vulkan/Utils/VkValidationLayerCallbacks.cpp
        Private/Application/ApplicationImpl.hpp
        Private/Application/Application.cpp
        Private/Window/Window_GLFW.cpp
        Private/Window/Window_GLFW.hpp
        Private/Window/WindowFactory.hpp
        Private/Renderer/Renderer.hpp
        Private/Renderer/Renderer.cpp)

##====================================
##  Target
##====================================

add_library(NeteroGraphics STATIC ${SRCS} ${PUBLIC_HEADER})
add_dependencies(NeteroGraphics Netero NeteroPatterns NeteroExtra)
target_compile_features(NeteroGraphics PUBLIC cxx_std_17)
target_include_directories(NeteroGraphics
        PUBLIC
        $<INSTALL_INTERFACE:include>
        $<BUILD_INTERFACE:${Vulkan_INCLUDE_DIRS}>
        $<BUILD_INTERFACE:${glm_SOURCE_DIR}>
        $<BUILD_INTERFACE:${Graphics_PUBLIC}>
        PRIVATE
        $<BUILD_INTERFACE:${glfw_INCLUDE_DIRS}>
        $<BUILD_INTERFACE:${Netero_Core_INCLUDE_DIRS}>
        $<BUILD_INTERFACE:${Netero_Patterns_INCLUDE_DIRS}>
        $<BUILD_INTERFACE:${Netero_Extra_INCLUDE_DIRS}>
        $<BUILD_INTERFACE:${Graphics_PRIVATE}>)
target_link_libraries(NeteroGraphics
        PUBLIC
        Netero
        NeteroPatterns
        NeteroExtra
        PRIVATE
        Vulkan::Vulkan
        glfw)

add_library(Netero::Graphics ALIAS NeteroGraphics)

##====================================
##  Cached Variables
##====================================

set(Netero_NewGraphics_FOUND True CACHE INTERNAL "Netero graphics FOUND flag.")

set(Netero_NewGraphics_INCLUDE_DIRS "${Graphics_PUBLIC}" CACHE INTERNAL "" FORCE)

set(Netero_INCLUDE_DIRS
        "${Netero_INCLUDE_DIRS}"
        "${Graphics_PUBLIC}" CACHE INTERNAL "Netero include directories.")

##====================================
##  Installation
##====================================

install(TARGETS NeteroGraphics
        EXPORT NeteroGraphics-export
        RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX}/lib
        LIBRARY DESTINATION ${CMAKE_INSTALL_PREFIX}/lib
        ARCHIVE DESTINATION ${CMAKE_INSTALL_PREFIX}/lib)
install(EXPORT NeteroGraphics-export
        FILE
        NeteroGraphicsTargets.cmake
        NAMESPACE
        Netero::
        DESTINATION
        ${CMAKE_INSTALL_PREFIX}/cmake/netero)
install(DIRECTORY Public/Netero
        DESTINATION ${CMAKE_INSTALL_PREFIX}/include
        FILES_MATCHING PATTERN "*.hpp")
