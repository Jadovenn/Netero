cmake_minimum_required (VERSION 3.11...3.16)
project(Tests
        VERSION 1.0
        DESCRIPTION "Netero unit test."
        LANGUAGES CXX)

message(STATUS "Configure Tests")

##====================================
##  Macro add_project_test
##====================================

macro(add_project_test)
    set( _OPTIONS_ARGS )
    set( _ONE_VALUE_ARGS NAME)
    set( _MULTI_VALUE_ARGS SOURCES DEPENDS)

    cmake_parse_arguments( _ADD_PROJECT_TEST "${_OPTIONS_ARGS}" "${_ONE_VALUE_ARGS}" "${_MULTI_VALUE_ARGS}" ${ARGN} )

    # Mandatory
    if(NOT _ADD_PROJECT_TEST_NAME OR NOT _ADD_PROJECT_TEST_SOURCES)
        message( FATAL_ERROR "add_project_test: missing argument(s)" )
    endif()

	add_executable(${_ADD_PROJECT_TEST_NAME} ${_ADD_PROJECT_TEST_SOURCES})
    target_compile_features(${_ADD_PROJECT_TEST_NAME} PUBLIC cxx_std_17)
	target_include_directories(${_ADD_PROJECT_TEST_NAME} PRIVATE ../include)
    if(WIN32_STATIC)
        set_property(TARGET ${_ADD_PROJECT_TEST_NAME} PROPERTY
            MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Release>:Release>")
    endif(WIN32_STATIC)
    if (_ADD_PROJECT_TEST_DEPENDS)
        add_dependencies(${_ADD_PROJECT_TEST_NAME} ${_ADD_PROJECT_TEST_DEPENDS})
        target_link_libraries(${_ADD_PROJECT_TEST_NAME} ${_ADD_PROJECT_TEST_DEPENDS})
    endif()
    if (CODE_COVERAGE)
        target_link_libraries(${_ADD_PROJECT_TEST_NAME} coverage_config)
    endif(CODE_COVERAGE)
	add_test(NAME ${_ADD_PROJECT_TEST_NAME} COMMAND ./${_ADD_PROJECT_TEST_NAME})
endmacro(add_project_test)

##====================================
##  Test Core
##====================================

add_project_test(NAME test_os SOURCES test_os.cpp DEPENDS netero)
add_project_test(NAME test_core_type_id SOURCES core/test_core_type_id.cpp)
add_project_test(NAME test_avl SOURCES test_avl.cpp)
add_project_test(NAME test_set SOURCES test_set.cpp)
add_project_test(NAME test_buffer SOURCES test_buffer.cpp)
add_project_test(NAME bug_size_buffer SOURCES bug_size_buffer.cpp)

##====================================
##  Test Audio
##====================================

if (NETERO_AUDIO)
    add_project_test(NAME test_audio_format SOURCES audio/test_audio_format.cpp DEPENDS netero neteroAudio)
    add_project_test(NAME example_audio_render SOURCES audio/example_audio_render.cpp DEPENDS netero neteroAudio)
    add_project_test(NAME example_audio_recorde SOURCES audio/example_audio_recorde.cpp DEPENDS netero neteroAudio)
    add_project_test(NAME example_audio_recorde_silence SOURCES audio/example_audio_recorde_silence.cpp DEPENDS netero neteroAudio)
endif(NETERO_AUDIO)

##====================================
##  Test Observer
##====================================

add_project_test(NAME test_slot SOURCES observer/test_slot_ref.cpp DEPENDS netero)
add_project_test(NAME test_signal_container SOURCES observer/test_signal_container.cpp DEPENDS netero)
add_project_test(NAME test_slot_container SOURCES observer/test_slot_container.cpp DEPENDS netero)

##====================================
##  Test MEMCHECK
##====================================

add_project_test(NAME test_allocator SOURCES memcheck/test_allocator.cpp DEPENDS Netero::Memcheck)
add_project_test(NAME test_new SOURCES memcheck/test_new.cpp DEPENDS Netero::Memcheck)

##====================================
##  Test ECS
##====================================

if(NETERO_ECS)
    add_project_test(NAME test_ecs_world SOURCES ecs/test_ecs_world.cpp DEPENDS neteroECS)
    add_project_test(NAME test_ecs_entity SOURCES ecs/test_ecs_entity.cpp DEPENDS neteroECS)
    add_project_test(NAME test_ecs_component SOURCES ecs/test_ecs_component.cpp DEPENDS neteroECS)
    add_project_test(NAME test_ecs_component_filter SOURCES ecs/test_ecs_component_filter.cpp DEPENDS neteroECS)
    add_project_test(NAME test_ecs_system
            SOURCES
                ecs/test_ecs_system.cpp
                ecs/test_ecs_dataset.hpp
            DEPENDS
                neteroECS)
endif(NETERO_ECS)

