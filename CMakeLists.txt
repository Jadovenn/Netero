cmake_minimum_required (VERSION 3.8)
project(Netero)

# Modules declaration
OPTION(CORE "netero Core, contain internal stuff" ON)
OPTION(ecs "netero ecs, contain Entity Component System architecture" ON)
OPTION(OBSERVER "netero Observer, contain a Qt signals/slots like, implementation" OFF)
OPTION(OS " netero Os, contain Operating System ressources encapsulation and abstraction" OFF)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
##set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

# Modules definitions

if(CORE)
	list(APPEND PUBLIC_HEADER
			./headers/netero/netero.hpp
			./headers/netero/core/internal.hpp
			./headers/netero/core/type_id.hpp
            ./headers/netero/avl.hpp
            ./headers/netero/set.hpp)
endif(CORE)

if(ecs)
	list(APPEND PUBLIC_HEADER
			./headers/netero/ecs/world.hpp
			./headers/netero/ecs/entity.hpp
			./headers/netero/ecs/component.hpp
			./headers/netero/ecs/component_filter.hpp
			./headers/netero/ecs/system.hpp)
	list(APPEND SRCS
			./sources/ecs/world.cpp
			./sources/ecs/entity.cpp
			./sources/ecs/entityContainer.cpp)
endif(ecs)

if(OS)
endif(OS)

if(OBSERVER)
endif(OBSERVER)

# Target

add_library(Netero ${SRCS} ${PUBLIC_HEADER})
target_include_directories(Netero PUBLIC headers)

set_target_properties(Netero
    PROPERTIES
	PUBLIC_HEADER "${PUBLIC_HEADER}"
	LINKER_LANGUAGE CXX
	CXX_STANDARD 17)

set(NETERO_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/headers)
enable_testing()
add_subdirectory("test")
