##
## Netero sources under BSD-3-Clause
## see LICENSE.txt
##

cmake_minimum_required(VERSION 3.11...3.16)

find_package(Vulkan)
if (NOT Vulkan_FOUND)
    message(FATAL_ERROR "Please install vulkan SDK.")
endif ()

if (NOT VULKAN_PATH)
    set(VULKAN_PATH $ENV{VULKAN_SDK})
    if (APPLE)
        set(VULKAN_PATH "/usr/local")
    endif (APPLE)
endif (NOT VULKAN_PATH)

if (NOT VULKAN_PATH)
    message(FATAL_ERROR "Please specify VULKAN_SDK environment variable or set VULKAN_PATH")
endif (NOT VULKAN_PATH)

set(FOUND_GLSLC FALSE)

if (WIN32)
    set(GLSLC_PATH "${VULKAN_PATH}\\Bin\\glslc.exe")
    if (EXISTS "${GLSLC_PATH}")
        set(FOUND_GLSLC TRUE)
    else ()
        set(GLSLC_PATH "${VULKAN_PATH}\\Bin32\\glslc.exe")
        if (EXISTS "${GLSLC_PATH}")
            set(FOUND_GLSLC TRUE)
        endif ()
    endif (EXISTS "${GLSLC_PATH}")
endif (WIN32)

if (APPLE)
    set(GLSLC_PATH "${VULKAN_PATH}/bin/glslc")
    if (EXISTS "${GLSLC_PATH}")
        set(FOUND_GLSLC TRUE)
    endif (EXISTS "${GLSLC_PATH}")
endif (APPLE)

if (FOUND_GLSLC)
    ##message(INFO " FOUND glslc ${GLSLC_PATH}")
else ()
    message(ERROR " NOT FOUND glslc")
endif ()

## Compile and move shader to the bin directory
function(target_export_shader TARGET SHADER_PATH)
    file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/shaders)
    get_filename_component(SHADER ${SHADER_PATH} NAME_WE)
    set(CURRENT_SHADER_OUTPUT_PATH ${CMAKE_CURRENT_BINARY_DIR}/shaders/${SHADER}.spv)
    set(CURRENT_SHADER_PATH ${CMAKE_CURRENT_SOURCE_DIR}/${SHADER_PATH})
    add_custom_command(
            OUTPUT ${CURRENT_SHADER_OUTPUT_PATH}
            COMMAND ${GLSLC_PATH} ${CURRENT_SHADER_PATH} -o ${CURRENT_SHADER_OUTPUT_PATH}
            DEPENDS ${CURRENT_SHADER_PATH}
            IMPLICIT_DEPENDS CXX ${CURRENT_SHADER_PATH}
            VERBATIM)
    set_source_files_properties(${CURRENT_SHADER_OUTPUT_PATH} PROPERTIES GENERATED TRUE)
    target_sources(${TARGET} PRIVATE ${CURRENT_SHADER_OUTPUT_PATH})
    if (APPLE)
        set_source_files_properties(${CURRENT_SHADER_OUTPUT_PATH} PROPERTIES
                MACOSX_PACKAGE_LOCATION "Shaders")
    endif (APPLE)
endfunction(target_export_shader)

function(target_add_shader TARGET SHADER_PATH)
    file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/shaders)
    get_filename_component(SHADER ${SHADER_PATH} NAME_WE)
    get_filename_component(SHADER_PATH_ONLY ${SHADER_PATH} DIRECTORY)
    set(CURRENT_SHADER_OUTPUT_PATH ${CMAKE_CURRENT_SOURCE_DIR}/${SHADER_PATH_ONLY}/${SHADER}.h)
    set(CURRENT_SHADER_PATH ${CMAKE_CURRENT_SOURCE_DIR}/${SHADER_PATH})
    add_custom_command(
            OUTPUT ${CURRENT_SHADER_OUTPUT_PATH}
            COMMAND ${GLSLC_PATH} -mfmt=c ${CURRENT_SHADER_PATH} -o ${CURRENT_SHADER_OUTPUT_PATH}
            DEPENDS ${CURRENT_SHADER_PATH}
            IMPLICIT_DEPENDS CXX ${CURRENT_SHADER_PATH}
            VERBATIM)
    set_source_files_properties(${CURRENT_SHADER_OUTPUT_PATH} PROPERTIES GENERATED TRUE)
    target_sources(${TARGET} PRIVATE ${CURRENT_SHADER_OUTPUT_PATH})
endfunction(target_add_shader)

function(target_add_shaders)
    cmake_parse_arguments(
            PARSED_ARGS
            ""
            "TARGET"
            "SOURCES"
            ${ARGN}
    )
    if(NOT PARSED_ARGS_TARGET)
        message(FATAL_ERROR "target_add_shaders: You must provide a target")
    endif(NOT PARSED_ARGS_TARGET)
    foreach(SHADER ${PARSED_ARGS_SOURCES})
        target_add_shader(${PARSED_ARGS_TARGET} ${SHADER})
    endforeach()
endfunction(target_add_shaders)

## move texture/image to the bin directory
function(target_export_texture TARGET TEXTURE_PATH)
    file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/textures)
    get_filename_component(TEXTURE ${TEXTURE_PATH} NAME)
    set(TEXTURE_OUTPUT_PATH ${CMAKE_CURRENT_BINARY_DIR}/textures/${TEXTURE})
    set(CURRENT_TEXTURE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/${TEXTURE_PATH})
    add_custom_command(
            OUTPUT ${TEXTURE_OUTPUT_PATH}
            COMMAND ${CMAKE_COMMAND} -E copy ${CURRENT_TEXTURE_PATH} ${TEXTURE_OUTPUT_PATH}
    )
    set_source_files_properties(${TEXTURE_OUTPUT_PATH} PROPERTIES GENERATED TRUE)
    target_sources(${TARGET} PRIVATE ${TEXTURE_OUTPUT_PATH})
    if (APPLE)
        set_source_files_properties(${TEXTURE_OUTPUT_PATH} PROPERTIES
                MACOSX_PACKAGE_LOCATION "Textures")
    endif (APPLE)
endfunction(target_export_texture)

function(target_export_resource TARGET BUNDLE_PATH RESOURCE_PATH)
    file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/Resources/${BUNDLE_PATH})
    get_filename_component(RESOURCE ${RESOURCE_PATH} NAME)
    set(RESOURCE_OUTPUT_PATH ${CMAKE_CURRENT_BINARY_DIR}/Resources/${BUNDLE_PATH}/${RESOURCE})
    set(CURRENT_RESOURCE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/${RESOURCE_PATH})
    add_custom_command(
            OUTPUT ${RESOURCE_OUTPUT_PATH}
            COMMAND ${CMAKE_COMMAND} -E copy ${CURRENT_RESOURCE_PATH} ${RESOURCE_OUTPUT_PATH}
    )
    set_source_files_properties(${RESOURCE_OUTPUT_PATH} PROPERTIES GENERATED TRUE)
    target_sources(${TARGET} PRIVATE ${RESOURCE_OUTPUT_PATH})
    if (APPLE)
        set_source_files_properties(${RESOURCE_OUTPUT_PATH} PROPERTIES
                MACOSX_PACKAGE_LOCATION "Resources/${BUNDLE_PATH}")
    endif (APPLE)
endfunction(target_export_resource)
