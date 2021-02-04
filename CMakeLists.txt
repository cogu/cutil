cmake_minimum_required(VERSION 3.14)

project(cutil LANGUAGES C VERSION 1.1.0)

if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
    include(../adt/cmake/BuildTypes.cmake)
    include(../adt/cmake/LeakCheck.cmake)
    include(../adt/cmake/UnitTest.cmake)
endif()

set(BYTE_ORDER AUTO CACHE STRING "Select byte order")
set_property(CACHE BYTE_ORDER PROPERTY STRINGS NONE AUTO LITTLE BIG)

if( (BYTE_ORDER STREQUAL "LITTLE") OR (BYTE_ORDER EQUAL 0))
    set(BYTE_ORDER_VALUE 0)
elseif( (BYTE_ORDER STREQUAL "BIG") OR (BYTE_ORDER EQUAL 1) )
    set(BYTE_ORDER_VALUE 1)
elseif( BYTE_ORDER STREQUAL "AUTO" )
    include(TestBigEndian)
    TEST_BIG_ENDIAN(BYTE_ORDER_VALUE)
    message(STATUS "BYTE_ORDER_VALUE=${BYTE_ORDER_VALUE}")
elseif (NOT BYTE_ORDER STREQUAL "NONE")
    message(FATAL_ERROR "Unsupported value '${BYTE_ORDER}' in BYTE_ORDER property")
endif()

if (UNIT_TEST)
    message(STATUS "UNIT_TEST=${UNIT_TEST} (CUTIL)")
endif()

if (LEAK_CHECK)
    message(STATUS "LEAK_CHECK=${LEAK_CHECK} (CUTIL)")
endif()

#### Library cutest
add_library(cutest cutest/CuTest.c cutest/CuTest.h)

target_include_directories(cutest PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/cutest
)

if(MSVC)
    target_compile_definitions(cutest PUBLIC _CRT_SECURE_NO_WARNINGS)
endif()

###

### Library cutil
set (CUTIL_HEADER_LIST
    ${CMAKE_CURRENT_SOURCE_DIR}/inc/argparse.h
    ${CMAKE_CURRENT_SOURCE_DIR}/inc/filestream.h
    ${CMAKE_CURRENT_SOURCE_DIR}/inc/pack.h
    ${CMAKE_CURRENT_SOURCE_DIR}/inc/sha256.h
    ${CMAKE_CURRENT_SOURCE_DIR}/inc/soa_chunk.h
    ${CMAKE_CURRENT_SOURCE_DIR}/inc/soa_fsa.h
    ${CMAKE_CURRENT_SOURCE_DIR}/inc/soa.h
)

set (CUTIL_SOURCE_LIST
    ${CMAKE_CURRENT_SOURCE_DIR}/src/argparse.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/filestream.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/pack.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/sha256.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/soa_chunk.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/soa_fsa.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/soa.c
)

if (LEAK_CHECK)
    list (APPEND CUTIL_HEADER_LIST ${CMAKE_CURRENT_SOURCE_DIR}/inc/CMemLeak.h)
    list (APPEND CUTIL_SOURCE_LIST ${CMAKE_CURRENT_SOURCE_DIR}/src/CMemLeak.c)
endif()

add_library(cutil ${CUTIL_SOURCE_LIST} ${CUTIL_HEADER_LIST})
target_link_libraries(cutil PRIVATE adt)
target_include_directories(cutil PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/inc)
if(DEFINED BYTE_ORDER_VALUE)
    target_compile_definitions(cutil PUBLIC PLATFORM_BYTE_ORDER=${BYTE_ORDER_VALUE})
endif()

if (UNIT_TEST)
    target_compile_definitions(cutil PRIVATE UNIT_TEST)
endif()

if (LEAK_CHECK)
    target_compile_definitions(cutil PRIVATE MEM_LEAK_CHECK)
endif()

###

### Executable cutil_unit


if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
    add_subdirectory(../adt ${CMAKE_CURRENT_BINARY_DIR}/adt)

    set (CUTIL_TEST_SUITE_LIST
        test/testsuite_argparse.c
        test/testsuite_pack.c
        test/testsuite_sha256.c
        test/testsuite_soa_fsa.c
    )

    add_executable(cutil_unit test/test_main.c ${CUTIL_TEST_SUITE_LIST})
    target_link_libraries(cutil_unit PRIVATE adt cutil cutest)
    target_include_directories(cutil_unit PRIVATE
                              "${PROJECT_BINARY_DIR}"
                              "${CMAKE_CURRENT_SOURCE_DIR}/inc"
                              "${CMAKE_CURRENT_SOURCE_DIR}/test"
                              )
    target_compile_definitions(cutil_unit PRIVATE UNIT_TEST)

    if (LEAK_CHECK)
        target_compile_definitions(cutil_unit PRIVATE MEM_LEAK_CHECK)
    endif()

    enable_testing()
    add_test(cutil_test cutil_unit)
    set_tests_properties(cutil_test PROPERTIES PASS_REGULAR_EXPRESSION "OK \\([0-9]+ tests\\)")

endif()
###

