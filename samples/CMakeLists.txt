cmake_minimum_required (VERSION 3.11...3.16)
project(netero_samples
    VERSION 1.0
    DESCRIPTION "Netero samples programmes."
    LANGUAGES CXX)

add_subdirectory(os)
add_subdirectory(memcheck)

if(NETERO_AUDIO)
    add_subdirectory(audio)
endif(NETERO_AUDIO)

if (NETERO_GRAPHICS)
    add_subdirectory(graphicsSurfaces)
    add_subdirectory(VikingRoom)
endif(NETERO_GRAPHICS)

