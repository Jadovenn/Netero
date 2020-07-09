cmake_minimum_required (VERSION 3.11...3.16)
project(NeteroTests
        VERSION 1.0
        DESCRIPTION "Netero unit test."
        LANGUAGES CXX)

add_unit_test(NAME avl_test SOURCES test_avl.cpp INCLUDE_DIRS ${Netero_INCLUDE_DIRS} DEPENDS Netero::Netero)
add_unit_test(NAME set_test SOURCES test_set.cpp INCLUDE_DIRS ${Netero_INCLUDE_DIRS} DEPENDS Netero::Netero)
add_unit_test(NAME buffer_test SOURCES test_buffer.cpp INCLUDE_DIRS ${Netero_INCLUDE_DIRS} DEPENDS Netero::Netero)

add_unit_test(NAME type_id_test SOURCES test_type_id.cpp INCLUDE_DIRS ${Netero_INCLUDE_DIRS} DEPENDS Netero::Netero)


## NOTE:
##  - OS Shoud be a sample not a test

