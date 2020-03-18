project(Tests)

set(NETERO_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/headers)
set(NETERO_LINK_LIBRARY netero)

## Audio test
if (AUDIO)
add_executable(test_audio_poll audio/test_audio_poll.cpp)
target_include_directories(test_audio_poll PUBLIC NETERO_INCLUDE_DIR)
target_link_libraries(test_audio_poll ${NETERO_LINK_LIBRARY})
add_test(NAME test_audio_poll COMMAND ./test_audio_poll)
set_target_properties(test_audio_poll
        PROPERTIES
        LINKER_LANGUAGE CXX
        CXX_STANDARD 17)

add_executable(test_audio audio/test_audio.cpp)
target_include_directories(test_audio PUBLIC NETERO_INCLUDE_DIR)
target_link_libraries(test_audio ${NETERO_LINK_LIBRARY})
add_test(NAME test_audio COMMAND ./test_audio)
set_target_properties(test_audio
        PROPERTIES
        LINKER_LANGUAGE CXX
        CXX_STANDARD 17)
endif(AUDIO)

## Observer test

add_executable(test_slot_ref observer/test_slot_ref.cpp)
target_include_directories(test_slot_ref PUBLIC NETERO_INCLUDE_DIR)
target_link_libraries(test_slot_ref ${NETERO_LINK_LIBRARY})
add_test(NAME test_slot_ref COMMAND ./test_slot_ref)
set_target_properties(test_slot_ref
        PROPERTIES
        LINKER_LANGUAGE CXX
        CXX_STANDARD 17)

add_executable(test_signals observer/test_signals.cpp)
target_include_directories(test_signals PUBLIC NETERO_INCLUDE_DIR)
target_link_libraries(test_signals ${NETERO_LINK_LIBRARY})
add_test(NAME test_signals COMMAND ./test_signals)
set_target_properties(test_signals
        PROPERTIES
        LINKER_LANGUAGE CXX
        CXX_STANDARD 17)

add_executable(test_slot_class_call observer/test_class_call.cpp)
target_include_directories(test_slot_class_call PUBLIC NETERO_INCLUDE_DIR)
target_link_libraries(test_slot_class_call ${NETERO_LINK_LIBRARY})
add_test(NAME test_slot_class_call COMMAND ./test_slot_class_call)
set_target_properties(test_slot_class_call
        PROPERTIES
        LINKER_LANGUAGE CXX
        CXX_STANDARD 17)

add_executable(test_slot_func_call observer/test_func_call.cpp)
target_include_directories(test_slot_func_call PUBLIC NETERO_INCLUDE_DIR)
target_link_libraries(test_slot_func_call ${NETERO_LINK_LIBRARY})
add_test(NAME test_slot_func_call COMMAND ./test_slot_func_call)
set_target_properties(test_slot_func_call
        PROPERTIES
        LINKER_LANGUAGE CXX
        CXX_STANDARD 17)

## set Test
add_executable(test_set test_set.cpp)
target_include_directories(test_set PUBLIC NETERO_INCLUDE_DIR)
target_link_libraries(test_set ${NETERO_LINK_LIBRARY})
add_test(NAME test_set COMMAND ./test_set)
set_target_properties(test_set
        PROPERTIES
        LINKER_LANGUAGE CXX
        CXX_STANDARD 17)

## AVL Test
add_executable(test_avl test_avl.cpp)
target_include_directories(test_avl PUBLIC NETERO_INCLUDE_DIR)
target_link_libraries(test_avl ${NETERO_LINK_LIBRARY})
add_test(NAME test_avl COMMAND ./test_avl)
set_target_properties(test_avl
        PROPERTIES
        LINKER_LANGUAGE CXX
        CXX_STANDARD 17)

# core Test
add_executable(test_core_type_id core/test_core_type_id.cpp)
target_include_directories(test_core_type_id PUBLIC NETERO_INCLUDE_DIR)
target_link_libraries(test_core_type_id ${NETERO_LINK_LIBRARY})
add_test(NAME test_core_type_id COMMAND ./test_core_type_id)
set_target_properties(test_core_type_id
        PROPERTIES
        LINKER_LANGUAGE CXX
        CXX_STANDARD 17)

# ECS Test
add_executable(test_ecs_world ecs/test_ecs_world.cpp)
target_include_directories(test_ecs_world PUBLIC NETERO_INCLUDE_DIR)
target_link_libraries(test_ecs_world ${NETERO_LINK_LIBRARY})
add_test(NAME test_ecs_world COMMAND ./test_ecs_world)
set_target_properties(test_ecs_world
        PROPERTIES
        LINKER_LANGUAGE CXX
        CXX_STANDARD 17)

add_executable(test_ecs_entity ecs/test_ecs_entity.cpp)
target_include_directories(test_ecs_entity PUBLIC NETERO_INCLUDE_DIR)
target_link_libraries(test_ecs_entity ${NETERO_LINK_LIBRARY})
add_test(NAME test_ecs_entity COMMAND ./test_ecs_entity)
set_target_properties(test_ecs_entity
        PROPERTIES
        LINKER_LANGUAGE CXX
        CXX_STANDARD 17)

add_executable(test_ecs_component ecs/test_ecs_component.cpp)
target_include_directories(test_ecs_component PUBLIC NETERO_INCLUDE_DIR)
target_link_libraries(test_ecs_component ${NETERO_LINK_LIBRARY})
add_test(NAME test_ecs_component COMMAND ./test_ecs_component)
set_target_properties(test_ecs_component
        PROPERTIES
        LINKER_LANGUAGE CXX
        CXX_STANDARD 17)

add_executable(test_ecs_component_filter ecs/test_ecs_component_filter.cpp)
target_include_directories(test_ecs_component_filter PUBLIC NETERO_INCLUDE_DIR)
target_link_libraries(test_ecs_component_filter ${NETERO_LINK_LIBRARY})
add_test(NAME test_ecs_component_filter COMMAND ./test_ecs_component)
set_target_properties(test_ecs_component_filter
        PROPERTIES
        LINKER_LANGUAGE CXX
        CXX_STANDARD 17)

add_executable(test_ecs_system ecs/test_ecs_system.cpp ecs/test_ecs_dataset.hpp)
target_include_directories(test_ecs_system PUBLIC NETERO_INCLUDE_DIR)
target_link_libraries(test_ecs_system ${NETERO_LINK_LIBRARY})
add_test(NAME test_ecs_system COMMAND ./test_ecs_system)
set_target_properties(test_ecs_system
        PROPERTIES
        LINKER_LANGUAGE CXX
        CXX_STANDARD 17)
