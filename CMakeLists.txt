cmake_minimum_required (VERSION 3.11...3.16)
project(netero
    VERSION 1.0
    DESCRIPTION "Real time utility library"
    LANGUAGES CXX)

##====================================
##  Init Project if not a dependency
##====================================

if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
    set(CMAKE_CXX_EXTENTIONS OFF)
    set_property(GLOBAL PROPERTY USE_FOLDERS ON)

    set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Real time utility library.")
    set(CPACK_PACKAGE_VENDOR "Netero")
    set(CPACK_PACKAGE_DESCRIPTION_FILE "${CMAKE_CURRENT_SOURCE_DIR}/README.md")
    set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/LICENSE.txt")
    set(CPACK_PACKAGE_VERSION_MAJOR "0")
    set(CPACK_PACKAGE_VERSION_MINOR "0")
    set(CPACK_PACKAGE_VERSION_PATCH "1")
    set(CPACK_PACKAGE_INSTALL_DIRECTORY "CMake ${CMake_VERSION_MAJOR}.${CMake_VERSION_MINOR}")
    if(WIN32 AND NOT UNIX)
      # There is a bug in NSI that does not handle full UNIX paths properly.
      # Make sure there is at least one set of four backlashes.
      set(CPACK_PACKAGE_ICON "${CMake_SOURCE_DIR}/Utilities/Release\\\\InstallIcon.bmp")
      set(CPACK_NSIS_INSTALLED_ICON_NAME "bin\\\\MyExecutable.exe")
      set(CPACK_NSIS_DISPLAY_NAME "${CPACK_PACKAGE_INSTALL_DIRECTORY} My Famous Project")
      set(CPACK_NSIS_HELP_LINK "http:\\\\\\\\www.my-project-home-page.org")
      set(CPACK_NSIS_URL_INFO_ABOUT "http:\\\\\\\\www.my-personal-home-page.com")
      set(CPACK_NSIS_CONTACT "me@my-personal-home-page.com")
      set(CPACK_NSIS_MODIFY_PATH ON)
    else()
      set(CPACK_STRIP_FILES "bin/MyExecutable")
      set(CPACK_SOURCE_STRIP_FILES "")
    endif()
    set(CPACK_PACKAGE_EXECUTABLES "MyExecutable" "My Executable")


    ##find_package(Doxygen)
    include(CTest)
    include(CPack)
    enable_testing()
    include(cmake/add_test_project.cmake)
    if(Doxygen_FOUND)
        add_subdirectory(docs)
    else()
        message(STATUS "Doxygen not found, not building docs")
    endif(Doxygen_FOUND)
    source_group(TREE "${PROJECT_SOURCE_DIR}/include" PREFIX "Header Files" FILES ${PUBLIC_HEADER})
endif()

# FetchContent added in CMake 3.11, downloads during the configure step
include(FetchContent)
if(${CMAKE_VERSION} VERSION_LESS 3.14)
    include(cmake/add_FetchContent_MakeAvailable.cmake)
endif()

add_compile_options("$<$<CONFIG:RELEASE>:-O2>")

##====================================
##  Modules declaration
##====================================

## Submodule config
OPTION(NETERO_AUDIO "Audio module library." ON)
OPTION(NETERO_GRAPHICS "Graphics module library." ON)
OPTION(NETERO_PATTERNS "Patterns module library." ON)
OPTION(NETERO_EXTRA "Self hosted STB lib components." ON)
OPTION(NETERO_MEMCHECK "Memory checker library." ON)

## Other config
OPTION(NETERO_UNIT_TEST "Netero unit tests." OFF)
OPTION(NETERO_SAMPLES "Netero samples." OFF)

## Build config mainly for CI
OPTION(WIN32_STATIC "Link statically with win32's dll" OFF)
OPTION(CODE_COVERAGE "Enable coverage reporting" OFF)
OPTION(MOCK_INTERFACES "Enable mock interfaces" OFF)

if(CODE_COVERAGE AND CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    # Add required flags (GCC & LLVM/Clang)
    add_library(coverage_config INTERFACE)
    target_compile_options(coverage_config INTERFACE
            -O0
            -g
            --coverage
            )
    if(CMAKE_VERSION VERSION_GREATER_EQUAL 3.13)
        target_link_options(coverage_config INTERFACE --coverage)
    else()
        target_link_libraries(coverage_config INTERFACE --coverage)
    endif()
endif()

if (NETERO_GRAPHICS)
    include(cmake/add_graphics_function.cmake)
endif(NETERO_GRAPHICS)

##====================================
##  Sub-project
##====================================

add_subdirectory(modules/core)
if (NETERO_AUDIO)
    add_subdirectory(modules/audio)
endif(NETERO_AUDIO)

if (NETERO_PATTERNS)
    add_subdirectory(modules/patterns)
endif(NETERO_PATTERNS)

if (NETERO_EXTRA)
    add_subdirectory(modules/extra)
endif(NETERO_EXTRA)

if(NETERO_GRAPHICS)
    if(NOT NETERO_EXTRA)
        message(FATAL_ERROR "GRAPHICS rely on EXTRA, EXTRA must be activated")
    endif(NOT NETERO_EXTRA)
    add_subdirectory(modules/graphics)
endif(NETERO_GRAPHICS)

if(NETERO_MEMCHECK)
    add_subdirectory(modules/memcheck)
endif(NETERO_MEMCHECK)

if(NETERO_SAMPLES)
    add_subdirectory(samples)
endif(NETERO_SAMPLES)

##====================================
##  Global install rules
##====================================

install(FILES cmake/Netero-config.cmake
        DESTINATION ${CMAKE_INSTALL_PREFIX}/cmake)
