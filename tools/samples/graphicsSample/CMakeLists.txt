cmake_minimum_required (VERSION 3.11...3.16)
project(GraphicSample
    VERSION 1.0
    DESCRIPTION "Graphics example program using netero_graphics."
    LANGUAGES CXX)

message(STATUS "Configure Netero Sample graphics")

set(SOURCES
        sources/main.cpp)

include_directories(${PROJECT_NAME} sources ../../../include)

if(WIN32)
    add_executable(${PROJECT_NAME} WindowsRessources.rc ${SOURCES})
endif(WIN32)
if(APPLE)
    set(MACOSX_BUNDLE_ICON_FILE NeteroIcon.icns)
    set(APP_ICON ${CMAKE_CURRENT_SOURCE_DIR}/NeteroIcon.icns)
    set_source_files_properties(${APP_ICON} PROPERTIES
            MACOSX_PACKAGE_LOCATION "Resources")
    add_executable(${PROJECT_NAME} MACOSX_BUNDLE ${APP_ICON} ${SOURCES})
endif(APPLE)

target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_17)
target_link_libraries(${PROJECT_NAME} netero neteroGraphics)
target_add_shader(${PROJECT_NAME} shaders/triangleVertices.vert)
target_add_shader(${PROJECT_NAME} shaders/triangleFragment.frag)
target_add_texture(${PROJECT_NAME} textures/cat.png)

