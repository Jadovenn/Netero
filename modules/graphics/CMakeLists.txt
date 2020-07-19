cmake_minimum_required (VERSION 3.11...3.16)
project(NeteroGraphics
        VERSION 0.1.0
        DESCRIPTION "Netero graphics module"
        LANGUAGES CXX)

message(STATUS "Configure Netero Grahpics module.")

##====================================
##  Extern Dependencies
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

set(GRAPHICS_PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/private)
set(GRAPHICS_PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/public)

list(APPEND PUBLIC_HEADER
        public/netero/graphics/window.hpp
        public/netero/graphics/application.hpp
        public/netero/graphics/context.hpp
        public/netero/graphics/device.hpp
        public/netero/graphics/axis.hpp
        public/netero/graphics/instance.hpp
        public/netero/graphics/model.hpp
        public/netero/graphics/descriptor_set.hpp
        public/netero/graphics/shader.hpp
        public/netero/graphics/vertex.hpp
        public/netero/graphics/texture.hpp
        public/netero/graphics/pipeline.hpp)

list(APPEND SRCS
        private/application.cpp
        private/context.cpp
        private/vk_device.cpp
        private/vk_descriptor_set.cpp
        private/vk_texture.cpp
        private/vk_vertex_buffer.cpp
        private/pipeline/pipeline.cpp
        private/pipeline/vk_pipeline_uniform_buffer.cpp
        private/model/model.cpp
        private/model/vk_model_pipeline.cpp
        private/utils/vkUtils.hpp
        private/utils/vkCheckValidationLayerSupport.cpp
        private/utils/vkExtentionLoaders.cpp
        private/utils/vkDebugCallbacks.cpp
        private/utils/vkFindQueueFamilies.cpp
        private/utils/vkSwapChainSupport.cpp
        private/utils/vkCommandBuffer.cpp
        private/utils/vkMemory.cpp
        private/utils/vkTransferCommand.cpp)

##====================================
##  Target
##====================================

add_library(NeteroGraphics STATIC ${SRCS} ${PUBLIC_HEADER})
add_dependencies(NeteroGraphics Netero NeteroExtra)
target_compile_features(NeteroGraphics PUBLIC cxx_std_17)
target_include_directories(NeteroGraphics
        PUBLIC
        $<INSTALL_INTERFACE:include>
        $<BUILD_INTERFACE:${Vulkan_INCLUDE_DIRS}>
        $<BUILD_INTERFACE:${glm_SOURCE_DIR}>
        $<BUILD_INTERFACE:${GRAPHICS_PUBLIC}>
        PRIVATE
        $<BUILD_INTERFACE:${glfw_INCLUDE_DIRS}>
        $<BUILD_INTERFACE:${Netero_Core_INCLUDE_DIRS}>
        $<BUILD_INTERFACE:${Netero_Extra_INCLUDE_DIRS}>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/private>)
target_link_libraries(NeteroGraphics
        PUBLIC
            Netero
            NeteroExtra
        PRIVATE
            Vulkan::Vulkan
            glfw)

add_library(Netero::Graphics ALIAS NeteroGraphics)

##====================================
##  Cached Variables
##====================================

set(Netero_Graphics_FOUND True CACHE INTERNAL "Netero graphics FOUND flag.")

set(Netero_Graphics_INCLUDE_DIRS "${Graphics_PUBLIC}" CACHE INTERNAL "" FORCE)

set(Netero_INCLUDE_DIRS
        "${Netero_INCLUDE_DIRS}"
        "${GRAPHICS_PUBLIC}" CACHE INTERNAL "Netero include directories.")

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
install(DIRECTORY public/netero
        DESTINATION ${CMAKE_INSTALL_PREFIX}/include
        FILES_MATCHING PATTERN "*.hpp")

