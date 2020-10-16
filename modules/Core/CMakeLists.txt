cmake_minimum_required(VERSION 3.11...3.16)
project(netero
        VERSION 1.0
        DESCRIPTION "Real time utility library"
        LANGUAGES CXX)

message(STATUS "Configure Netero libs")


##====================================
##  Sources
##====================================

set(CORE_PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/private)
set(CORE_PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/public)

list(APPEND PUBLIC_HEADER
        ## Core
        Public/Netero/Netero.hpp
        Public/Netero/Debug.hpp
        Public/Netero/Logger.hpp
        Public/Netero/Exception.hpp
        Public/Netero/TypeId.hpp
        ## Algo
        Public/Netero/Avl.hpp
        Public/Netero/Set.hpp
        Public/Netero/Buffer.hpp
        ## OS
        Public/Netero/Os.hpp
        )

list(APPEND SRCS
        Private/Logger/Logger.cpp)

##====================================
##  Os sources
##====================================

if (MOCK_INTERFACES)
    list(APPEND SRCS
            Private/Os/MockOsHelpers.cpp)
else ()
    if (WIN32)
        list(APPEND SRCS
                Private/Os/WindowsOsHelpers.cpp)
        list(APPEND LINK_LIBRARIES
                Userenv)
    endif (WIN32)
    if (APPLE)
        list(APPEND SRCS
                Private/Os/MacOsHelpers.cpp)
    endif (APPLE)
    if (UNIX AND NOT APPLE)
        list(APPEND SRCS
                Private/Os/UnixOsHelpers.cpp)
    endif (UNIX AND NOT APPLE)
endif (MOCK_INTERFACES)


##====================================
##  Targets
##====================================

if (APPLE)
    FIND_LIBRARY(COREFOUNDATION_LIBRARY CoreFoundation)
    list(APPEND LINK_LIBRARIES
            ${COREFOUNDATION_LIBRARY})
endif (APPLE)

add_library(Netero STATIC ${SRCS} ${PUBLIC_HEADER})
target_compile_features(Netero PUBLIC cxx_std_17)
target_include_directories(Netero
        PUBLIC
        $<INSTALL_INTERFACE:include>
        $<BUILD_INTERFACE:${CORE_PUBLIC}>
        )
target_link_libraries(Netero ${LINK_LIBRARIES})

if (WIN32 AND WIN32_STATIC)
    set_property(TARGET Netero PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Release>:Release>")
endif ()

add_library(Netero::Netero ALIAS Netero)

##====================================
##  Global Variables
##====================================

set(Netero_FOUND True CACHE INTERNAL "Netero FOUND flag.")

set(Netero_Core_INCLUDE_DIRS "${CORE_PUBLIC}" CACHE INTERNAL "" FORCE)

set(Netero_INCLUDE_DIRS
        "${CORE_PUBLIC}" CACHE INTERNAL "Netero include directories.")
mark_as_advanced(Netero_INCLUDE_DIRS)

##====================================
##  Install rules
##====================================

install(TARGETS Netero
        EXPORT Netero-export
        RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX}/lib
        LIBRARY DESTINATION ${CMAKE_INSTALL_PREFIX}/lib
        ARCHIVE DESTINATION ${CMAKE_INSTALL_PREFIX}/lib)
install(EXPORT Netero-export
        FILE
        NeteroTargets.cmake
        NAMESPACE
        Netero::
        DESTINATION
        ${CMAKE_INSTALL_PREFIX}/cmake/netero)

install(DIRECTORY public/netero
        DESTINATION ${CMAKE_INSTALL_PREFIX}/include
        FILES_MATCHING PATTERN "*.hpp")

##====================================
##  Tests
##====================================

if (BUILD_TESTING AND NETERO_UNIT_TEST)
    add_subdirectory(tests)
endif ()

