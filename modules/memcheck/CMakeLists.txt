cmake_minimum_required (VERSION 3.11...3.16)
project(neteroMemcheck
        VERSION 1.0
        DESCRIPTION "Lightway memory checker."
        LANGUAGES CXX)

message(STATUS "Configure Netero Memcheck")

##====================================
##  Core Sources
##====================================

list(APPEND PUBLIC_HEADER
        ${HEADERS_DIRECTORY}/memcheck/block.hpp
        ${HEADERS_DIRECTORY}/memcheck/memory.hpp
        ${HEADERS_DIRECTORY}/memcheck/report.hpp)

list(APPEND SRCS
        report.cpp)

add_library(neteroMemcheck STATIC ${SRCS} ${PUBLIC_HEADER})
add_library(Netero::Memcheck ALIAS neteroMemcheck)
target_include_directories(neteroMemcheck
    PUBLIC
        $<INSTALL_INTERFACE:include>
        $<BUILD_INTERFACE:${INCLUDE_DIRECTORY_PATH}>
)

target_compile_features(neteroMemcheck PUBLIC cxx_std_17)

##====================================
##  Install rules
##====================================

install(TARGETS neteroMemcheck
        EXPORT memcheck-export
        RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX}/lib
        LIBRARY DESTINATION ${CMAKE_INSTALL_PREFIX}/lib
        ARCHIVE DESTINATION ${CMAKE_INSTALL_PREFIX}/lib)
install(EXPORT memcheck-export
        FILE
            memcheckTargets.cmake
        NAMESPACE
            Netero::
        DESTINATION
            ${CMAKE_INSTALL_PREFIX}/lib/cmake/netero)
install(DIRECTORY ${INCLUDE_DIRECTORY_PATH}/netero/memcheck
        DESTINATION ${CMAKE_INSTALL_PREFIX}/include/netero
        FILES_MATCHING PATTERN "*.hpp")

