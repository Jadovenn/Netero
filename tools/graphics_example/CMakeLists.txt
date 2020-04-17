cmake_minimum_required (VERSION 3.11...3.16)
project(GraphicsExample
	VERSION 1.0
	DESCRIPTION "Graphics example program using netero_graphics."
	LANGUAGES CXX)

message(STATUS "Configure Netero GraphicsExample")

set(SOURCES
		sources/main.cpp
		WindowsRessources.rc)

include_directories(${PROJECT_NAME} sources ../../include)
add_executable(${PROJECT_NAME} ${SOURCES})
target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_17)
target_link_libraries(${PROJECT_NAME} netero_graphics)

