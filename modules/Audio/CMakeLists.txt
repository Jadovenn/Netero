cmake_minimum_required(VERSION 3.11...3.16)
project(netero_audio
        VERSION 0.1.0
        DESCRIPTION "Netero audio module"
        LANGUAGES CXX)

message(STATUS "Configure Netero Audio module")

##====================================
##  Sources
##====================================

set(AUDIO_PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/private)
set(AUDIO_PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/public)

list(APPEND PUBLIC_HEADER
        Public/Netero/Audio/DeviceManager.hpp
        Public/Netero/Audio/Device.hpp
        Public/Netero/Audio/Wave.hpp
        Public/Netero/Audio/WaveFile.hpp)

list(APPEND SRCS
        private/WaveFile.cpp)

##====================================
##  OS dependent sources
##====================================

if (WIN32)
    list(APPEND SRCS
            Private/Backends/Wasapi/WasapiHelpers.hpp
            Private/Backends/Wasapi/WasapiHelpers.cpp
            Private/Backends/Wasapi/DeviceManagerImpl.hpp
            Private/Backends/Wasapi/DeviceManagerImpl.cpp
            Private/Backends/Wasapi/DeviceManagerScanForDevice.cpp
            Private/Backends/Wasapi/DeviceImpl.hpp
            Private/Backends/Wasapi/DeviceImpl.cpp
            Private/Backends/Wasapi/DeviceImplAcquisition.cpp
            Private/Backends/Wasapi/DeviceImplRenderer.cpp)
    list(APPEND LINK_LIBRARIES
            avrt)
endif (WIN32)

if (APPLE)
    FIND_LIBRARY(COREFOUNDATION_LIBRARY CoreFoundation)
    FIND_LIBRARY(COREAUDIO_LIBRARY CoreAudio)
    list(APPEND SRCS
            Private/Backends/CoreAudio/DeviceManagerImpl.hpp
            Private/Backends/CoreAudio/DeviceManagerImpl.cpp
            Private/Backends/CoreAudio/DeviceManagerScanForDevice.cpp
            Private/Backends/CoreAudio/DeviceImpl.hpp
            Private/Backends/CoreAudio/DeviceImpl.cpp)
    list(APPEND LINK_LIBRARIES
            ${COREAUDIO_LIBRARY}
            ${COREFOUNDATION_LIBRARY})
endif (APPLE)

##====================================
##  Target
##====================================

add_library(NeteroAudio STATIC ${SRCS} ${PUBLIC_HEADER})
target_compile_features(NeteroAudio PUBLIC cxx_std_17)
add_dependencies(NeteroAudio Netero::Netero Netero::Patterns)
target_include_directories(NeteroAudio
        PUBLIC
        $<INSTALL_INTERFACE:include>
        $<BUILD_INTERFACE:${AUDIO_PUBLIC}>
        PRIVATE
        $<BUILD_INTERFACE:${Netero_Core_INCLUDE_DIRS}>
        $<BUILD_INTERFACE:${Netero_Patterns_INCLUDE_DIRS}>)
target_link_libraries(NeteroAudio
        PUBLIC
        Netero::Patterns
        PRIVATE
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

