get_filename_component(netero_CMAKE_DIR "${CMAKE_CURRENT_LIST_FILE}" PATH)
include(CMakeFindDependencyMacro)

if(NOT TARGET Netero::netero)
    include("${netero_CMAKE_DIR}/cmake/netero/neteroTargets.cmake")
    foreach(component ${noms_FIND_COMPONENTS})
        include(${netero_CMAKE_DIR}/cmake/netero/${component}Targets.cmake)
    endforeach()
endif()
