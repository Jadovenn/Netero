cmake_minimum_required (VERSION 3.11...3.16)
project(netero_core
        VERSION 1.0
        DESCRIPTION "Real time utility library"
        LANGUAGES CXX)

message(STATUS "Configure Netero libs")


##====================================
##  Core Sources
##====================================

list(APPEND PUBLIC_HEADER
        ${HEADERS_DIRECTORY}/netero.hpp
        ${HEADERS_DIRECTORY}/type_id.hpp)

##====================================
##  Algorithms Sources
##====================================

list(APPEND PUBLIC_HEADER
        ${HEADERS_DIRECTORY}/avl.hpp
        ${HEADERS_DIRECTORY}/set.hpp
        ${HEADERS_DIRECTORY}/buffer.hpp)

##====================================
##  Pool sources
##====================================

list(APPEND PUBLIC_HEADER
        ${HEADERS_DIRECTORY}/pool/worker.hpp)
list(APPEND SRCS
        pool/worker.cpp)

##====================================
##  Oberver sources
##====================================
list(APPEND PUBLIC_HEADER
        ${HEADERS_DIRECTORY}/observer/IObserverDelegate.hpp
        ${HEADERS_DIRECTORY}/observer/signal.hpp
        ${HEADERS_DIRECTORY}/observer/slot.hpp)

##====================================
##  Os sources
##====================================
list(APPEND PUBLIC_HEADER
        ${HEADERS_DIRECTORY}/os.hpp)
if(MOCK_INTERFACES)
    list(APPEND SRCS
        os/mockOsHelpers.cpp)
else()
    if(WIN32)
        list(APPEND SRCS
            os/windowsOsHelpers.cpp)
        list(APPEND LINK_LIBRARIES
            Userenv)
    endif(WIN32)
    if(APPLE)
        list(APPEND SRCS
                os/macOsHelpers.cpp)
    endif(APPLE)
    if(UNIX AND NOT APPLE)
        list(APPEND SRCS
                os/unixOsHelpers.cpp)
    endif(UNIX AND NOT APPLE)
endif(MOCK_INTERFACES)


##====================================
##  Targets
##====================================
if (APPLE)
    FIND_LIBRARY(COREFOUNDATION_LIBRARY CoreFoundation)
    list(APPEND LINK_LIBRARIES
            ${COREFOUNDATION_LIBRARY})
endif(APPLE)

add_library(netero STATIC ${SRCS} ${PUBLIC_HEADER})
add_library(Netero::Core ALIAS netero)
target_include_directories(netero
	PUBLIC
		$<INSTALL_INTERFACE:include>
		$<BUILD_INTERFACE:${INCLUDE_DIRECTORY_PATH}>
)
target_include_directories(netero
        PUBLIC
            $<INSTALL_INTERFACE:include>
            $<BUILD_INTERFACE:${INCLUDE_DIRECTORY_PATH}>
)
target_link_libraries(netero ${LINK_LIBRARIES})
if(WIN32 AND WIN32_STATIC)
    set_property(TARGET netero PROPERTY
        MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Release>:Release>")
endif()

target_compile_features(netero PUBLIC cxx_std_17)

##====================================
##  Install rules
##====================================

install(TARGETS netero
        EXPORT netero-export
        RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX}/lib
        LIBRARY DESTINATION ${CMAKE_INSTALL_PREFIX}/lib
        ARCHIVE DESTINATION ${CMAKE_INSTALL_PREFIX}/lib)
install(EXPORT netero-export
        FILE
            neteroTargets.cmake
        NAMESPACE
            Netero::
        DESTINATION
            ${CMAKE_INSTALL_PREFIX}/lib/cmake/netero)

## Core directory
install(FILES
            ${HEADERS_DIRECTORY}/netero.hpp
            ${HEADERS_DIRECTORY}/avl.hpp
            ${HEADERS_DIRECTORY}/set.hpp
        DESTINATION
            ${CMAKE_INSTALL_PREFIX}/include/netero)
install(DIRECTORY ${INCLUDE_DIRECTORY_PATH}/netero/core
		DESTINATION ${CMAKE_INSTALL_PREFIX}/include/netero
		FILES_MATCHING PATTERN "*.hpp")
install(DIRECTORY ${INCLUDE_DIRECTORY_PATH}/netero/pool
        DESTINATION ${CMAKE_INSTALL_PREFIX}/include/netero
        FILES_MATCHING PATTERN "*.hpp")
install(DIRECTORY ${INCLUDE_DIRECTORY_PATH}/netero/observer
        DESTINATION ${CMAKE_INSTALL_PREFIX}/include/netero
        FILES_MATCHING PATTERN "*.hpp")

