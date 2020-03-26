cmake_minimum_required (VERSION 3.11...3.16)
project(Tests
        VERSION 1.0
        DESCRIPTION "Netero unit test."
        LANGUAGES CXX)

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
    if (_ADD_PROJECT_TEST_DEPENDS)
        add_dependencies(${_ADD_PROJECT_TEST_NAME} ${_ADD_PROJECT_TEST_DEPENDS})
        target_link_libraries(${_ADD_PROJECT_TEST_NAME} ${_ADD_PROJECT_TEST_DEPENDS})
    endif()
	add_test(NAME ${_ADD_PROJECT_TEST_NAME} COMMAND ./${_ADD_PROJECT_TEST_NAME})
endmacro(add_project_test)

##====================================
##  Test Core
##====================================

add_project_test(NAME test_core_type_id SOURCES core/test_core_type_id.cpp)
add_project_test(NAME test_avl SOURCES test_avl.cpp)
add_project_test(NAME test_set SOURCES test_set.cpp)

##====================================
##  Test Audio
##====================================

if (AUDIO)
    add_project_test(NAME test_audio_format SOURCES audio/test_audio_format.cpp DEPENDS netero_audio)
    add_project_test(NAME test_audio_mixer SOURCES audio/test_audio_mixer.cpp DEPENDS netero_audio)
endif(AUDIO)

##====================================
##  Test Observer
##====================================

if(OBSERVER)
    add_project_test(NAME test_slot SOURCES observer/test_slot_ref.cpp DEPENDS netero)
    add_project_test(NAME test_signals SOURCES observer/test_signals.cpp DEPENDS netero)
    add_project_test(NAME test_slot_class_call SOURCES observer/test_class_call.cpp DEPENDS netero)
    add_project_test(NAME test_slot_func_call SOURCES observer/test_func_call.cpp DEPENDS netero)
endif(OBSERVER)

##====================================
##  Test ECS
##====================================

if(ECS)
    add_project_test(NAME test_ecs_world SOURCES ecs/test_ecs_world.cpp DEPENDS netero_ecs_static)
    add_project_test(NAME test_ecs_entity SOURCES ecs/test_ecs_entity.cpp DEPENDS netero_ecs_static)
    add_project_test(NAME test_ecs_component SOURCES ecs/test_ecs_component.cpp DEPENDS netero_ecs_static)
    add_project_test(NAME test_ecs_component_filter SOURCES ecs/test_ecs_component_filter.cpp DEPENDS netero_ecs_static)
    add_project_test(NAME test_ecs_system
            SOURCES
                ecs/test_ecs_system.cpp
                ecs/test_ecs_dataset.hpp
            DEPENDS
                netero_ecs_static)
endif(ECS)
