cmake_minimum_required(VERSION 3.14)

project(cutil LANGUAGES C VERSION 1.0)

if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
    include(../adt/cmake/BuildTypes.cmake)
    include(../adt/cmake/LeakCheck.cmake)
    include(../adt/cmake/UnitTest.cmake)
endif()

set(BYTE_ORDER AUTO CACHE STRING "Select byte order")
set_property(CACHE BYTE_ORDER PROPERTY STRINGS NONE AUTO LITTLE BIG)

if(BYTE_ORDER STREQUAL "LITTLE")
    set(BYTE_ORDER_VALUE 0)
elseif(BYTE_ORDER STREQUAL "BIG")
    set(BYTE_ORDER_VALUE 1)
elseif(BYTE_ORDER STREQUAL "AUTO")
    include(TestBigEndian)
    TEST_BIG_ENDIAN(BYTE_ORDER_VALUE)
    message(STATUS "BYTE_ORDER_VALUE=${BYTE_ORDER_VALUE}")
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
    inc/filestream.h
    inc/pack.h
    inc/sha256.h
    inc/soa_chunk.h
    inc/soa_fsa.h
    inc/soa.h
)

set (CUTIL_SOURCE_LIST
    src/filestream.c
    src/pack.c
    src/sha256.c
    src/soa_chunk.c
    src/soa_fsa.c
    src/soa.c
)

if (LEAK_CHECK)
    list (APPEND CUTIL_HEADER_LIST inc/CMemLeak.h)
    list (APPEND CUTIL_SOURCE_LIST src/CMemLeak.c)
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

endif()
###

