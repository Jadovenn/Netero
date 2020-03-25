get_filename_component(netero_CMAKE_DIR "${CMAKE_CURRENT_LIST_FILE}" PATH)
include(CMakeFindDependencyMacro)

if(NOT TARGET Netero::netero)
    include("${netero_CMAKE_DIR}/neteroTargets.cmake")
endif()