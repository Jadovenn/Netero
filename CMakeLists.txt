cmake_minimum_required (VERSION 3.8)
project(Netero)

# Modules declaration
OPTION(CORE "netero Core, contain internal stuff" ON)
OPTION(ECS "netero ECS, contain Entity Component System architecture" ON)
OPTION(OBSERVER "netero Observer, contain a Qt signals/slots like, implementation" OFF)
OPTION(OS " netero Os, contain Operating System ressources encapsulation and abstraction" OFF)

# Modules definitions

if(CORE)
	list(APPEND PUBLIC_HEADER
			./Headers/netero/netero.hpp
			./Headers/netero/Core/internal.hpp)
endif(CORE)

if(ECS)
	list(APPEND PUBLIC_HEADER
			./Headers/netero/ECS/world.hpp)
	list(APPEND SRCS
			./Sources/ECS/world.cpp
)
endif(ECS)

if(OS)
endif(OS)

if(OBSERVER)
endif(OBSERVER)

# Target

add_library(Netero ${SRCS} ${PUBLIC_HEADER})
target_include_directories(Netero PUBLIC Headers)

set_target_properties(Netero
    PROPERTIES
	LINKER_LANGUAGE CXX
	CXX_STANDARD 17
)

# Test
enable_testing()
add_subdirectory("Test")
