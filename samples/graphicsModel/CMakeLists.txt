cmake_minimum_required(VERSION 3.11...3.16)
project(NeteroGraphicsModel
        VERSION 1.0
        DESCRIPTION "Graphics example program using netero_graphics."
        LANGUAGES CXX)

message(STATUS "Configure Netero Sample graphics")

set(SOURCES
        main.cpp)

include_directories(${PROJECT_NAME} ${Netero_INCLUDE_DIRS})

if (WIN32)
    if (WIN_APP)
        add_compile_definitions(WIN_APP)
        add_executable(${PROJECT_NAME} WIN32 WindowsResources.rc ${SOURCES})
    else ()
        add_executable(${PROJECT_NAME} WindowsResources.rc ${SOURCES})
    endif ()
endif (WIN32)
if (APPLE)
    set(MACOSX_BUNDLE_ICON_FILE NeteroIcon.icns)
    set(APP_ICON ${CMAKE_CURRENT_SOURCE_DIR}/NeteroIcon.icns)
    set_source_files_properties(${APP_ICON} PROPERTIES
            MACOSX_PACKAGE_LOCATION "Resources")
    add_executable(${PROJECT_NAME} MACOSX_BUNDLE ${APP_ICON} ${SOURCES})
endif (APPLE)

target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_17)
target_link_libraries(${PROJECT_NAME} Netero::Netero Netero::Graphics Netero::Extra)
target_add_shader(${PROJECT_NAME} shaders/VikingHouseVertexShader.vert)
target_add_shader(${PROJECT_NAME} shaders/VikingHouseFragmentShader.frag)
target_add_resource(${PROJECT_NAME} vikingHouseAsset object/viking_house.OBJ)
target_add_resource(${PROJECT_NAME} vikingHouseAsset object/viking_house.png)

