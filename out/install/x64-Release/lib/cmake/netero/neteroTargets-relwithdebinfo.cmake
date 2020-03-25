#----------------------------------------------------------------
# Generated CMake target import file for configuration "RelWithDebInfo".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "Netero::netero" for configuration "RelWithDebInfo"
set_property(TARGET Netero::netero APPEND PROPERTY IMPORTED_CONFIGURATIONS RELWITHDEBINFO)
set_target_properties(Netero::netero PROPERTIES
  IMPORTED_IMPLIB_RELWITHDEBINFO "C:/Users/jean-/source/repos/netero/out/install/x64-Release/lib/netero.lib"
  )

list(APPEND _IMPORT_CHECK_TARGETS Netero::netero )
list(APPEND _IMPORT_CHECK_FILES_FOR_Netero::netero "C:/Users/jean-/source/repos/netero/out/install/x64-Release/lib/netero.lib" )

# Import target "Netero::netero_static" for configuration "RelWithDebInfo"
set_property(TARGET Netero::netero_static APPEND PROPERTY IMPORTED_CONFIGURATIONS RELWITHDEBINFO)
set_target_properties(Netero::netero_static PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_RELWITHDEBINFO "CXX"
  IMPORTED_LOCATION_RELWITHDEBINFO "C:/Users/jean-/source/repos/netero/out/install/x64-Release/lib/netero_static.lib"
  )

list(APPEND _IMPORT_CHECK_TARGETS Netero::netero_static )
list(APPEND _IMPORT_CHECK_FILES_FOR_Netero::netero_static "C:/Users/jean-/source/repos/netero/out/install/x64-Release/lib/netero_static.lib" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
