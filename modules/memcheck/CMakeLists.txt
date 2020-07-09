cmake_minimum_required (VERSION 3.11...3.16)
project(neteroMemcheck
        VERSION 1.0
        DESCRIPTION "Lightway memory checker."
        LANGUAGES CXX)

message(STATUS "Configure Netero Memcheck")

##====================================
##  Core Sources
##====================================

set(MEMCHECK_PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/private)
set(MEMCHECK_PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/public)

list(APPEND PUBLIC_HEADER
        public/netero/memcheck/block.hpp
        public/netero/memcheck/new.hpp)

list(APPEND SRCS
        private/new.cpp)

add_library(NeteroMemcheck STATIC ${SRCS} ${PUBLIC_HEADER})
add_dependencies(NeteroMemcheck Netero)
target_compile_features(NeteroMemcheck PUBLIC cxx_std_17)
target_link_libraries(NeteroMemcheck Netero)
target_include_directories(NeteroMemcheck
    PUBLIC
        $<INSTALL_INTERFACE:include>
        $<BUILD_INTERFACE:${MEMCHECK_PUBLIC}>
    PRIVATE
        $<BUILD_INTERFACE:${Netero_INCLUDE_DIRS}>
)

add_library(Netero::Memcheck ALIAS NeteroMemcheck)

##====================================
##  Install rules
##====================================

install(TARGETS NeteroMemcheck
        EXPORT NeteroMemcheck-export
        RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX}/lib
        LIBRARY DESTINATION ${CMAKE_INSTALL_PREFIX}/lib
        ARCHIVE DESTINATION ${CMAKE_INSTALL_PREFIX}/lib)
install(EXPORT NeteroMemcheck-export
        FILE
            NeteroMemcheckTargets.cmake
        NAMESPACE
            Netero::
        DESTINATION
            ${CMAKE_INSTALL_PREFIX}/cmake/netero)
install(DIRECTORY public/netero/memcheck
        DESTINATION ${CMAKE_INSTALL_PREFIX}/include/netero
        FILES_MATCHING PATTERN "*.hpp")

