cmake_minimum_required (VERSION 3.11...3.16)
project(netero_audio
        VERSION 0.1.0
        DESCRIPTION "Netero audio submodule"
        LANGUAGES CXX)

##====================================
##  Generic core
##====================================

list(APPEND PUBLIC_HEADER
        ${HEADERS_DIRECTORY}/audio/audio.hpp
        ${HEADERS_DIRECTORY}/audio/engine.hpp
        ${HEADERS_DIRECTORY}/audio/mixer.hpp
        ${HEADERS_DIRECTORY}/audio/device.hpp
        ${HEADERS_DIRECTORY}/audio/signals.hpp)
list(APPEND SRCS
        mixer.cpp
        device.cpp
        signals/sinusoid.cpp)

##====================================
##  Windows core
##====================================

if(WIN32)
    list(APPEND SRCS
            engines/win32/WASAPI.hpp
            engines/win32/WASAPI.cpp
            engines/win32/WASAPI_init.cpp
            engines/win32/WASAPI_poll.cpp
            engines/win32/WASAPI_async.cpp)
    list(APPEND LINK_LIBRARIES
            avrt)
endif(WIN32)

##====================================
##  APPLE core
##====================================

if (APPLE)
    FIND_LIBRARY(COREAUDIO_LIBRARY CoreAudio)
    list(APPEND SRCS
                engines/CoreAudio/CoreAudio.hpp
                engines/CoreAudio/CoreAudio.cpp
                engines/CoreAudio/CoreAudio_init.cpp
                engines/CoreAudio/CoreAudio_poll.cpp
                engines/CoreAudio/CoreAudio_async.cpp)
        list(APPEND LINK_LIBRARIES
                ${COREAUDIO_LIBRARY})
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
            ${LIBRARIES})

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
