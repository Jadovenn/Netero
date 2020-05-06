cmake_minimum_required (VERSION 3.11...3.16)

find_package(Vulkan)
if (NOT Vulkan_FOUND)
    message(FATAL_ERROR "Please install vulkan SDK.")
endif()

if (NOT VULKAN_PATH)
    set(VULKAN_PATH $ENV{VULKAN_SDK})
endif(NOT VULKAN_PATH)

if (NOT VULKAN_PATH)
    message(FATAL_ERROR "Please specify VULKAN_SDK environment variable or set VULKAN_PATH")
endif(NOT VULKAN_PATH)

set(FOUND_GLSLC FALSE)

if (WIN32)
    set(GLSLC_PATH "${VULKAN_PATH}\\Bin\\glslc.exe")
    if(EXISTS "${GLSLC_PATH}")
        set(FOUND_GLSLC TRUE)
    else()
        set(GLSLC_PATH "${VULKAN_PATH}\\Bin32\\glslc.exe")
        if(EXISTS "${GLSLC_PATH}")
            set(FOUND_GLSLC TRUE)
        endif()
    endif(EXISTS "${GLSLC_PATH}")
endif(WIN32)

if (FOUND_GLSLC)
    message(INFO " FOUND glslc ${GLSLC_PATH}")
else()
    message(ERROR " NOT FOUND glslc")
endif()

function(target_add_shader TARGET SHADER_PATH)
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
endfunction(target_add_shader)

