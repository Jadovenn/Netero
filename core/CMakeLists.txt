cmake_minimum_required (VERSION 3.11...3.16)
project(netero_core
        VERSION 1.0
        DESCRIPTION "Real time utility library"
        LANGUAGES CXX)

message(STATUS "Configure CORE features")

##====================================
##  Core Sources
##====================================

list(APPEND PUBLIC_HEADER
        ${HEADERS_DIRECTORY}/netero.hpp
        ${HEADERS_DIRECTORY}/core/internal.hpp
        ${HEADERS_DIRECTORY}/core/type_id.hpp)

##====================================
##  Algorithms Sources
##====================================

list(APPEND PUBLIC_HEADER
        ${HEADERS_DIRECTORY}/avl.hpp
        ${HEADERS_DIRECTORY}/set.hpp
        ${HEADERS_DIRECTORY}/buffer.hpp)

##====================================
##  Exceptions Sources
##====================================

list(APPEND PUBLIC_HEADER
        ${HEADERS_DIRECTORY}/exception.hpp
        ${HEADERS_DIRECTORY}/exception/slots.hpp)

##====================================
##  Pool sources
##====================================

list(APPEND PUBLIC_HEADER
        ${HEADERS_DIRECTORY}/pool/worker.hpp)
list(APPEND SRCS
        pool/worker.cpp)

##====================================
##  Pool sources
##====================================
list(APPEND PUBLIC_HEADER
        ${HEADERS_DIRECTORY}/observer/IObserverDelegate.hpp
        ${HEADERS_DIRECTORY}/observer/signals.hpp
        ${HEADERS_DIRECTORY}/observer/slots.hpp)

##====================================
##  Targets
##====================================

add_library(netero STATIC ${SRCS} ${PUBLIC_HEADER})
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
install(DIRECTORY ${INCLUDE_DIRECTORY_PATH}/netero/exception
        DESTINATION ${CMAKE_INSTALL_PREFIX}/include/netero
        FILES_MATCHING PATTERN "*.hpp")
install(DIRECTORY ${INCLUDE_DIRECTORY_PATH}/netero/pool
        DESTINATION ${CMAKE_INSTALL_PREFIX}/include/netero
        FILES_MATCHING PATTERN "*.hpp")
install(DIRECTORY ${INCLUDE_DIRECTORY_PATH}/netero/observer
        DESTINATION ${CMAKE_INSTALL_PREFIX}/include/netero
        FILES_MATCHING PATTERN "*.hpp")

