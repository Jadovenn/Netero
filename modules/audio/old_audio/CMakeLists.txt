cmake_minimum_required(VERSION 3.11...3.16)
project(netero_audio
        VERSION 0.1.0
        DESCRIPTION "Netero audio module"
        LANGUAGES CXX)

message(STATUS "Configure Netero Audio module")

##====================================
##  Sources
##====================================

set(AUDIO_PRIVATE private)
set(AUDIO_PUBLIC public)

list(APPEND PUBLIC_HEADER
        public/netero/audio/audio.hpp
        public/netero/audio/device.hpp
        public/netero/audio/backend.hpp
        public/netero/audio/deviceManager.hpp
        public/netero/audio/engine.hpp
        public/netero/audio/audioStream.hpp
        public/netero/audio/renderStream.hpp
        public/netero/audio/captureStream.hpp
        public/netero/audio/mixer.hpp
        public/netero/audio/signals.hpp
        public/netero/audio/format/wave.hpp
        public/netero/audio/format/waveRecorder.hpp)

list(APPEND SRCS
        private/deviceManager.cpp
        private/engine.cpp
        private/mixer.cpp
        private/renderStream.cpp
        private/captureStream.cpp
        private/format/waveRecorder.cpp
        private/signals/sinusoid.cpp)

##====================================
##  Windows sources
##====================================

if (WIN32)
    list(APPEND SRCS
            private/backends/Wasapi/WASAPI.hpp
            private/backends/Wasapi/WASAPI.cpp
            private/backends/Wasapi/WASAPI_device.cpp
            private/backends/Wasapi/WASAPI_default_devices.cpp
            private/backends/Wasapi/WASAPI_get_devices.cpp
            private/backends/Wasapi/WASAPI_render.cpp
            private/backends/Wasapi/WASAPI_capture.cpp)
    list(APPEND LINK_LIBRARIES
            avrt)
endif (WIN32)

##====================================
##  APPLE sources
##====================================

if (APPLE)
    FIND_LIBRARY(COREFOUNDATION_LIBRARY CoreFoundation)
    FIND_LIBRARY(COREAUDIO_LIBRARY CoreAudio)
    ## TODO: refactor CoreAudio impl for new netero::audio API
    ##list(APPEND SRCS
    ##        private/backends/CoreAudio/CoreAudio.hpp
    ##        private/backends/CoreAudio/CoreAudio.cpp
    ##        private/backends/CoreAudio/CoreAudio_get_format.cpp
    ##        private/backends/CoreAudio/CoreAudio_default_device.cpp
    ##        private/backends/CoreAudio/CoreAudio_start_stop.cpp)
    ##    list(APPEND LINK_LIBRARIES
    ##            ${COREAUDIO_LIBRARY}
    ##            ${COREFOUNDATION_LIBRARY})
endif (APPLE)

##====================================
##  Target
##====================================

add_library(NeteroAudio STATIC ${SRCS} ${PUBLIC_HEADER})
target_compile_features(NeteroAudio public cxx_std_17)
add_dependencies(NeteroAudio Netero NeteroPatterns)
target_include_directories(NeteroAudio
        public
        $<INSTALL_INTERFACE:include>
        $<BUILD_INTERFACE:${AUDIO_PUBLIC}>
        private
        $<BUILD_INTERFACE:${Netero_INCLUDE_DIRS}>)
target_link_libraries(NeteroAudio
        public
        Netero::Patterns
        private
        ${LINK_LIBRARIES})

if (WIN32 AND WIN32_STATIC)
    set_property(TARGET NeteroAudio PROPERTY
            MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Release>:Release>")
endif ()

add_library(Netero::Audio ALIAS NeteroAudio)

##====================================
##  Global Variables
##====================================

set(Netero_Audio_FOUND True CACHE INTERNAL "Netero Audio FOUND flag.")

set(Netero_INCLUDE_DIRS
        ${Netero_INCLUDE_DIRS}
        ${CORE_PUBLIC} CACHE INTERNAL "Netero include directories.")

##====================================
##  Installation
##====================================

install(TARGETS NeteroAudio
        EXPORT NeteroAudio-export
        RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX}/lib
        LIBRARY DESTINATION ${CMAKE_INSTALL_PREFIX}/lib
        ARCHIVE DESTINATION ${CMAKE_INSTALL_PREFIX}/lib)
install(EXPORT NeteroAudio-export
        FILE
        NeteroAudioTargets.cmake
        NAMESPACE
        Netero::
        DESTINATION
        ${CMAKE_INSTALL_PREFIX}/cmake/netero)
install(DIRECTORY public/netero/audio
        DESTINATION ${CMAKE_INSTALL_PREFIX}/include/netero
        FILES_MATCHING PATTERN "*.hpp")

