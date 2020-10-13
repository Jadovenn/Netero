
cmake_minimum_required (VERSION 3.11...3.16)

macro(add_unit_test)
    set( _OPTIONS_ARGS )
    set( _ONE_VALUE_ARGS NAME)
    set( _MULTI_VALUE_ARGS SOURCES INCLUDE_DIRS DEPENDS)

    cmake_parse_arguments( _ADD_UNIT_TEST "${_OPTIONS_ARGS}" "${_ONE_VALUE_ARGS}" "${_MULTI_VALUE_ARGS}" ${ARGN} )

    # Mandatory
    if(NOT _ADD_UNIT_TEST_NAME OR NOT _ADD_UNIT_TEST_SOURCES)
        message( FATAL_ERROR "add_project_test: missing argument(s)" )
    endif()

    add_executable(${_ADD_UNIT_TEST_NAME} ${_ADD_UNIT_TEST_SOURCES})
    target_compile_features(${_ADD_UNIT_TEST_NAME} PUBLIC cxx_std_17)
    target_include_directories(${_ADD_UNIT_TEST_NAME} PRIVATE ${_ADD_UNIT_TEST_INCLUDE_DIRS})
    if(WIN32_STATIC)
        set_property(TARGET ${_ADD_UNIT_TEST_NAME} PROPERTY
            MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Release>:Release>")
    endif(WIN32_STATIC)
    if (_ADD_UNIT_TEST_DEPENDS)
        add_dependencies(${_ADD_UNIT_TEST_NAME} ${_ADD_UNIT_TEST_DEPENDS})
        target_link_libraries(${_ADD_UNIT_TEST_NAME} ${_ADD_UNIT_TEST_DEPENDS})
    endif()
    if (CODE_COVERAGE)
        target_link_libraries(${_ADD_UNIT_TEST_NAME} coverage_config)
    endif(CODE_COVERAGE)
    add_test(NAME ${_ADD_UNIT_TEST_NAME} WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/bin COMMAND ./${_ADD_UNIT_TEST_NAME})
endmacro(add_unit_test)
