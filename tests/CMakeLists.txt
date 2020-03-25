project(Tests)

macro(netero_add_test test_name file_names)
	add_executable(${test_name} ${file_names})
	add_dependencies(${test_name} netero)
	target_include_directories(${test_name} PRIVATE ../headers)
	target_link_libraries(${test_name} netero)
	set_target_properties(${test_name}
			PROPERTIES
			LINKER_LANGUAGE CXX
			CXX_STANDARD 17)	
	add_test(NAME ${test_name} COMMAND ./${test_name})
endmacro(netero_add_test)

## Audio test
if (AUDIO)
	netero_add_test(test_audio_format audio/test_audio_format.cpp)
	netero_add_test(test_audio_mixer audio/test_audio_mixer.cpp)
	netero_add_test(test_audio_engine_poll audio/test_audio_engine_poll.cpp)
	netero_add_test(test_audio_engine_async audio/test_audio_engine_async.cpp)
endif(AUDIO)

## Observer test
if(OBSERVER)
	netero_add_test(test_slot_ref observer/test_slot_ref.cpp)
	netero_add_test(test_signals observer/test_signals.cpp)
	netero_add_test(test_slot_class_call observer/test_class_call.cpp)
	netero_add_test(test_slot_func_call observer/test_func_call.cpp)
endif(OBSERVER)

# core Test
netero_add_test(test_core_type_id core/test_core_type_id.cpp)
netero_add_test(test_avl test_avl.cpp)
netero_add_test(test_set test_set.cpp)

# ECS Test
if(ECS)
	netero_add_test(test_ecs_world ecs/test_ecs_world.cpp)
	netero_add_test(test_ecs_entiry ecs/test_ecs_entity.cpp)
	netero_add_test(test_ecs_component ecs/test_ecs_component.cpp)
	netero_add_test(test_ecs_component_filter ecs/test_ecs_component_filter.cpp)
	list(APPEND test_ecs_system_srcs
			ecs/test_ecs_system.cpp
			ecs/test_ecs_dataset.hpp)
	netero_add_test(test_ecs_system ${test_ecs_system_srcs})
endif(ECS)

