cmake_minimum_required (VERSION 3.11...3.16)
project(NeteroConsole
        VERSION 0.1.0
        DESCRIPTION "Netero console module"
        LANGUAGES CXX)

message(STATUS "Configure Netero Console module.")

##====================================
##  Sources
##====================================

set(CONSOLE_PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/Private)
set(CONSOLE_PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/Public)

set(PUBLIC_HEADER
        Public/Netero/ProgramArguments.hpp)

set(SRCS
        Private/ProgramArguments.cpp)

##====================================
##  Target
##====================================

add_library(NeteroConsole STATIC ${SRCS} ${PUBLIC_HEADER})
add_dependencies(NeteroConsole Netero)
target_compile_features(NeteroConsole PUBLIC cxx_std_17)
target_include_directories(NeteroConsole
        PUBLIC
            $<INSTALL_INTERFACE:include>
            $<BUILD_INTERFACE:${CONSOLE_PUBLIC}>
        PRIVATE
            $<BUILD_INTERFACE:${CONSOLE_PRIVATE}>)
target_link_libraries(NeteroConsole
        PUBLIC
            Netero)

add_library(Netero::Console ALIAS NeteroConsole)

##====================================
##  Cached Variables
##====================================

set(Netero_Console_FOUND True CACHE INTERNAL "Netero Console FOUND flag.")

set(Netero_Console_INCLUDE_DIRS "${CONSOLE_PUBLIC}" CACHE INTERNAL "" FORCE)

set(Netero_INCLUDE_DIRS
        "${Netero_INCLUDE_DIRS}"
        "${CONSOLE_PUBLIC}" CACHE INTERNAL "Netero include directories.")

##====================================
##  Installation
##====================================

install(TARGETS NeteroConsole
        EXPORT NeteroConsole-export
        RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX}/lib
        LIBRARY DESTINATION ${CMAKE_INSTALL_PREFIX}/lib
        ARCHIVE DESTINATION ${CMAKE_INSTALL_PREFIX}/lib)
install(EXPORT NeteroConsole-export
        FILE
        NeteroConsoleTargets.cmake
        NAMESPACE
        Netero::
        DESTINATION
        ${CMAKE_INSTALL_PREFIX}/cmake/Netero)
install(DIRECTORY Public/Netero
        DESTINATION ${CMAKE_INSTALL_PREFIX}/include
        FILES_MATCHING PATTERN "*.hpp")

##====================================
##  Tests
##====================================

if (BUILD_TESTING AND NETERO_UNIT_TEST)
    add_subdirectory(tests)
endif ()
