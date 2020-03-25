
set(HEADERS_DIRECTORY_PATH ${CMAKE_SOURCE_DIR}/include/netero)

list(APPEND PUBLIC_HEADER
		${HEADERS_DIRECTORY_PATH}/netero.hpp
		${HEADERS_DIRECTORY_PATH}/core/internal.hpp
		${HEADERS_DIRECTORY_PATH}/core/type_id.hpp
        ${HEADERS_DIRECTORY_PATH}/avl.hpp
        ${HEADERS_DIRECTORY_PATH}/set.hpp
        ${HEADERS_DIRECTORY_PATH}/exception.hpp
        ${HEADERS_DIRECTORY_PATH}/exception/slots.hpp)

if(ECS)
	list(APPEND PUBLIC_HEADER
		${HEADERS_DIRECTORY_PATH}/ecs/world.hpp
		${HEADERS_DIRECTORY_PATH}/ecs/entity.hpp
		${HEADERS_DIRECTORY_PATH}/ecs/component.hpp
		${HEADERS_DIRECTORY_PATH}/ecs/component_filter.hpp
		${HEADERS_DIRECTORY_PATH}/ecs/system.hpp)

	list(APPEND SRCS
		ecs/world.cpp
		ecs/entity.cpp
		ecs/entityContainer.cpp)
endif(ECS)

if(POOL)
    list(APPEND PUBLIC_HEADER
            ${HEADERS_DIRECTORY_PATH}/pool/worker.hpp)
    list(APPEND SRCS
            pool/worker.cpp)
endif(POOL)

if(OBSERVER)
    list(APPEND PUBLIC_HEADER
            ${HEADERS_DIRECTORY_PATH}/observer/IObserverDelegate.hpp
            ${HEADERS_DIRECTORY_PATH}/observer/signals.hpp
            ${HEADERS_DIRECTORY_PATH}/observer/slots.hpp)
endif(OBSERVER)

if(ENGINE)
endif(ENGINE)

if(AUDIO)
	list(APPEND PUBLIC_HEADER
			${HEADERS_DIRECTORY_PATH}/audio/audio.hpp
			${HEADERS_DIRECTORY_PATH}/audio/engine.hpp
			${HEADERS_DIRECTORY_PATH}/audio/mixer.hpp
			${HEADERS_DIRECTORY_PATH}/audio/device.hpp
			${HEADERS_DIRECTORY_PATH}/audio/signals.hpp)
	list(APPEND SRCS
			audio/mixer.cpp
			audio/device.cpp
			audio/signals/sinusoid.cpp)
	if(WIN32)
		list(APPEND SRCS
			audio/engines/win32/WASAPI.hpp
			audio/engines/win32/WASAPI.cpp
			audio/engines/win32/WASAPI_init.cpp
			audio/engines/win32/WASAPI_poll.cpp
			audio/engines/win32/WASAPI_async.cpp)
		list(APPEND LIBRARIES
			avrt)
	endif(WIN32)
endif(AUDIO)

add_library(netero SHARED ${SRCS} ${PUBLIC_HEADER})
add_library(netero_static STATIC ${SRCS} ${PUBLIC_HEADER})
target_include_directories(netero
	PUBLIC
		$<INSTALL_INTERFACE:include>
		$<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/include>
)
target_include_directories(netero_static
        PUBLIC
            $<INSTALL_INTERFACE:include>
            $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/include>
)

if(LIBRARIES)
	target_link_libraries(netero
		PRIVATE
			${LIBRARIES}
	)
    target_link_libraries(netero_static
            PRIVATE
                ${LIBRARIES}
    )
endif(LIBRARIES)

target_compile_features(netero PUBLIC cxx_std_17)
target_compile_features(netero_static PUBLIC cxx_std_17)

source_group(TREE "${PROJECT_SOURCE_DIR}/include" PREFIX "Header Files" FILES ${PUBLIC_HEADER})

install(TARGETS netero netero_static
        EXPORT netero-export
		RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX}/lib
        LIBRARY DESTINATION ${CMAKE_INSTALL_PREFIX}/lib
        ARCHIVE DESTINATION ${CMAKE_INSTALL_PREFIX}/lib
)
install(DIRECTORY ${CMAKE_SOURCE_DIR}/include
		DESTINATION ${CMAKE_INSTALL_PREFIX}/include
		FILES_MATCHING PATTERN "*.hpp")
install(FILES ../cmake/neteroConfig.cmake
        DESTINATION ${CMAKE_INSTALL_PREFIX}/lib)
install(EXPORT netero-export
	FILE
		neteroTargets.cmake
	NAMESPACE
		Netero::
	DESTINATION
		${CMAKE_INSTALL_PREFIX}/lib/cmake/netero
)
