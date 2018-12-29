cmake_minimum_required (VERSION 3.8)
project(Netero)

# Modules declaration
OPTION(CORE "netero Core, contain internal stuff" ON)
OPTION(ECS "netero ECS, contain Entity Component System architecture" ON)
OPTION(OBSERVER "netero Observer, contain a Qt signals/slots like, implementation" OFF)
OPTION(OS " netero Os, contain Operating System ressources encapsulation and abstraction" OFF)

set(CMAKE_CXX_COMPILER_VERSION  17)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
##set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

# Modules definitions

if(CORE)
	list(APPEND PUBLIC_HEADER
			./Headers/netero/netero.hpp
			./Headers/netero/Core/internal.hpp)
endif(CORE)

if(ECS)
	list(APPEND PUBLIC_HEADER
			./Headers/netero/ECS/world.hpp
			./Headers/netero/ECS/entity.hpp)
	list(APPEND SRCS
			./Sources/ECS/world.cpp
			./Sources/ECS/worldCache.cpp
			./Sources/ECS/entityContainer.cpp
			./Sources/ECS/entity.cpp)
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
	PUBLIC_HEADER "${PUBLIC_HEADER}"
	LINKER_LANGUAGE CXX
##	INTERFACE_INCLUDE_DIRECTORIES "C:/Program Files/Netero/include"
	CXX_STANDARD 17)


##INSTALL(TARGETS Netero
##		EXPORT NETEROConfig
##        LIBRARY DESTINATION "C:/Program Files/Netero/lib"
##		ARCHIVE DESTINATION "C:/Program Files/Netero/lib")
##export(TARGETS Netero
##		NAMESPACE Netero::
##		FILE ${CMAKE_CURRENT_BINARY_DIR}/NETEROConfig.cmake)
##INSTALL(EXPORT NETEROConfig
##		DESTINATION "C:/Program Files/Netero/lib/cmake/Netero"
##		NAMESPACE Netero::)
##INSTALL(DIRECTORY ${CMAKE_SOURCE_DIR}/Headers/netero
##		DESTINATION "C:/Program Files/Netero/include"
##		PATTERN "*.h*")


set(NETERO_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/Headers)
enable_testing()
add_subdirectory("Test")
