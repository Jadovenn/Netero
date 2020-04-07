cmake_minimum_required (VERSION 3.11...3.16)
project(netero_audio
        VERSION 0.1.0
        DESCRIPTION "Netero audio submodule"
        LANGUAGES CXX)

message(STATUS "Configure AUDIO submodule")

##====================================
##  Generic audio sources
##====================================

list(APPEND PUBLIC_HEADER
        ${HEADERS_DIRECTORY}/audio/audio.hpp
        ${HEADERS_DIRECTORY}/audio/backend.hpp
        ${HEADERS_DIRECTORY}/audio/engine.hpp
        ${HEADERS_DIRECTORY}/audio/renderStream.hpp
        ${HEADERS_DIRECTORY}/audio/captureStream.hpp
        ${HEADERS_DIRECTORY}/audio/device.hpp
        ${HEADERS_DIRECTORY}/audio/mixer.hpp
        ${HEADERS_DIRECTORY}/audio/signals.hpp
        ${HEADERS_DIRECTORY}/audio/format/wave.hpp
        ${HEADERS_DIRECTORY}/audio/format/waveRecorder.hpp)
list(APPEND SRCS
        engine.cpp
        mixer.cpp
        renderStream.cpp
        captureStream.cpp
        format/waveRecorder.cpp
        signals/sinusoid.cpp)

##====================================
##  Windows sources
##====================================

if(WIN32)
    list(APPEND SRCS
            backends/Wasapi/WASAPI.hpp
            backends/Wasapi/WASAPI.cpp
            backends/Wasapi/WASAPI_default_devices.cpp
            backends/Wasapi/WASAPI_get_devices.cpp
            backends/Wasapi/WASAPI_render.cpp
            backends/Wasapi/WASAPI_capture.cpp)
    list(APPEND LINK_LIBRARIES
            avrt)
endif(WIN32)

##====================================
##  APPLE sources
##====================================

if (APPLE)
    FIND_LIBRARY(COREFOUNDATION_LIBRARY CoreFoundation)
    FIND_LIBRARY(COREAUDIO_LIBRARY CoreAudio)
    ## TODO: refactor CoreAudio impl for new netero::audio API
    ##list(APPEND SRCS
    ##        backends/CoreAudio/CoreAudio.hpp
    ##        backends/CoreAudio/CoreAudio.cpp
    ##        backends/CoreAudio/CoreAudio_get_format.cpp
    ##        backends/CoreAudio/CoreAudio_default_device.cpp
    ##        backends/CoreAudio/CoreAudio_start_stop.cpp)
    ##    list(APPEND LINK_LIBRARIES
    ##            ${COREAUDIO_LIBRARY}
    ##            ${COREFOUNDATION_LIBRARY})
endif(APPLE)

##====================================
##  Target
##====================================

add_library(netero_audio STATIC ${SRCS} ${PUBLIC_HEADER})
target_compile_features(netero_audio PUBLIC cxx_std_17)
target_include_directories(netero_audio
        PUBLIC
        $<INSTALL_INTERFACE:include>
        $<BUILD_INTERFACE:${INCLUDE_DIRECTORY_PATH}>)
target_link_libraries(netero_audio
        PRIVATE
            ${LINK_LIBRARIES})

if(WIN32)
    set_property(TARGET netero_audio PROPERTY
        MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Release>:Release>")
endif(WIN32)

##====================================
##  Installation
##====================================

install(TARGETS netero_audio
        EXPORT audio-export
        RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX}/lib
        LIBRARY DESTINATION ${CMAKE_INSTALL_PREFIX}/lib
        ARCHIVE DESTINATION ${CMAKE_INSTALL_PREFIX}/lib)
install(EXPORT audio-export
        FILE
            audioTargets.cmake
        NAMESPACE
            Netero::
        DESTINATION
            ${CMAKE_INSTALL_PREFIX}/lib/cmake/netero)
install(DIRECTORY ${INCLUDE_DIRECTORY_PATH}/netero/audio
        DESTINATION ${CMAKE_INSTALL_PREFIX}/include/netero
        FILES_MATCHING PATTERN "*.hpp")

