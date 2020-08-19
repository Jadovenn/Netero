
set(ISPC_IA_TARGETS "sse2-i32x4,sse4-i32x8,avx1-i32x16,avx2-i32x16,avx512knl-i32x16,avx512skx-i32x16")

if (WIN32)

    set(ISPC_PATH "${CMAKE_CURRENT_SOURCE_DIR}/bin/ispc/win64/ispc.exe")
    set(ISPC_FLAGS "-o2 --target ${ISPC_ISA_TARGETS}")

elseif(APPLE)

    set(ISPC_PATH "${CMAKE_CURRENT_SOURCE_DIR}/bin/ispc/mach-o/ispc")
    set(ISPC_FLAGS "")

endif(WIN32)

macro(target_add_ispc_sources)
    set(_OPTIONS_ARG)
    set(_ONE_VALUE_ARGS TARGET)
    set(_MULTI_VALUE_ARGS ISPC_SOURCES)

    cmake_parse_arguments( _TARGET_ADD_ISPC_SOURCES "${_OPTIONS_ARGS}" "${_ONE_VALUE_ARGS}" "${_MULTI_VALUE_ARGS}" "${ARGN}")

    if(NOT _TARGET_ADD_ISPC_SOURCES_TARGET)
        message(FATAL_ERROR "You must specify a target")
    endif()
    if (NOT _TARGET_ADD_ISPC_SOURCES_ISPC_SOURCES)
        message(FATAL_ERROR "You must specify at least one source file")
    endif()

    foreach(ISPC_SOURCE ${_TARGET_ADD_ISPC_SOURCES_ISPC_SOURCES})
        get_filename_component(FILENAME ${ISPC_SOURCE} NAME_WE)
        get_filename_component(FILEDIR ${ISPC_SOURCE} DIRECTORY)
        set(OUTPUT_FILE "${CMAKE_BINARY_DIR}/${FILENAME}_ispc.o")
        set(HEADER_FILE "${CMAKE_CURRENT_SOURCE_DIR}/${FILEDIR}/${FILENAME}.h")
        add_custom_command(
            OUTPUT ${OUTPUT_FILE}
            COMMAND ${ISPC_PATH} ${ISPC_ISA_TARGET} ${CMAKE_CURRENT_SOURCE_DIR}/${ISPC_SOURCE} -o ${OUTPUT_FILE} -h ${HEADER_FILE}
        )
        target_sources(${_TARGET_ADD_ISPC_SOURCES_TARGET} PRIVATE ${OUTPUT_FILE})
    endforeach()

endmacro(target_add_ispc_sources)

