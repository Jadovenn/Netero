cmake_minimum_required(VERSION 3.11...3.16)
project(cube
        VERSION 1.0
        DESCRIPTION "Graphics example program using netero_graphics."
        LANGUAGES CXX)

message(STATUS "Configure Netero Surface Sample graphics")

set(SOURCES
        main.cpp)

include_directories(${PROJECT_NAME} ${Netero_INCLUDE_DIRS})

if (WIN32)
    if (WIN_APP)
        add_compile_definitions(WIN_APP)
        add_executable(${PROJECT_NAME} WIN32 WindowsRessources.rc ${SOURCES})
    else ()
        add_executable(${PROJECT_NAME} WindowsRessources.rc ${SOURCES})
    endif ()
endif (WIN32)
if (APPLE)
    set(MACOSX_BUNDLE_ICON_FILE NeteroIcon.icns)
    set(APP_ICON ${CMAKE_CURRENT_SOURCE_DIR}/NeteroIcon.icns)
    set_source_files_properties(${APP_ICON} PROPERTIES
            MACOSX_PACKAGE_LOCATION "Resources")
    add_executable(${PROJECT_NAME} MACOSX_BUNDLE ${APP_ICON} ${SOURCES})
    set_target_properties(${PROJECT_NAME} PROPERTIES
            XCODE_ATTRIBUTE_CODE_SIGN_ENTITLEMENTS
            "${CMAKE_CURRENT_SOURCE_DIR}/cube.entitlements")
endif (APPLE)

target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_17)
target_link_libraries(${PROJECT_NAME} Netero::Netero Netero::Graphics)
target_export_shader(${PROJECT_NAME} shaders/ModelVertex.vert)
target_export_shader(${PROJECT_NAME} shaders/ColorFragment.frag)
target_export_shader(${PROJECT_NAME} shaders/TexturedModelVertex.vert)
target_export_shader(${PROJECT_NAME} shaders/TexturedColorFragment.frag)
target_export_texture(${PROJECT_NAME} textures/cat.png)

