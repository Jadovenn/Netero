cmake_minimum_required (VERSION 3.11...3.16)
project(netero_graphics
        VERSION 0.1.0
        DESCRIPTION "Netero graphics submodule"
        LANGUAGES CXX)

message(STATUS "Configure Netero graphics submodule")

##====================================
##  Dependency
##====================================

set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)

find_package(Vulkan)
if(NOT Vulkan_FOUND)
    message(FATAL_ERROR "Please install vulkan SDK.")
endif()

FetchContent_Declare(
        glfw
        GIT_REPOSITORY https://github.com/glfw/glfw.git
)
FetchContent_MakeAvailable(glfw)
FetchContent_Declare(
	glm
	GIT_REPOSITORY https://github.com/g-truc/glm.git
)
FetchContent_GetProperties(glm)
if(NOT glm_POPULATED)
	FetchContent_Populate(glm)
	set(GLM_TEST_ENABLE OFF CACHE BOOL "" FORCE)
	add_subdirectory(${glm_SOURCE_DIR} ${glm_BINARY_DIR})
endif()

##====================================
##  Generic core
##====================================

list(APPEND PUBLIC_HEADER
        ${HEADERS_DIRECTORY}/graphics/application.hpp
        ${HEADERS_DIRECTORY}/graphics/context.hpp
        ${HEADERS_DIRECTORY}/graphics/shader.hpp
        ${HEADERS_DIRECTORY}/graphics/window.hpp)
list(APPEND SRCS
        application.cpp
        utils/vkUtils.hpp
        utils/vkCheckValidationLayerSupport.cpp
        utils/vkExtentionLoaders.cpp
        utils/vkDebugCallbacks.cpp
        utils/vkFindQueueFamilies.cpp
        utils/vkSwapChainSupport.cpp
        context/context.cpp
        context/physicalDevices.cpp
        context/graphicsPipeline.cpp
        context/shaders.cpp)

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
        $<BUILD_INTERFACE:${glm_SOURCE_DIR}>
        PRIVATE
        $<BUILD_INTERFACE:${glfw_INCLUDE_DIRS}>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>)
message(STATUS "GLM headers: ${glm_SOURCE_DIR}")
target_link_libraries(netero_graphics
        PRIVATE
            Vulkan::Vulkan
            glfw)
##====================================
##  Installation
##====================================

install(TARGETS netero_graphics
        EXPORT graphics-export
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
