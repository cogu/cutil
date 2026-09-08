cmake_minimum_required(VERSION 3.20)

project(cutil LANGUAGES C VERSION 1.3.0)

set(CMAKE_DISABLE_IN_SOURCE_BUILD ON)
set(CMAKE_DISABLE_SOURCE_CHANGES ON)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_OPTIMIZE_DEPENDENCIES ON)

if(MSVC)
    add_compile_definitions(_CRT_SECURE_NO_WARNINGS)
    add_compile_options(/W4)
    option(ENABLE_MSVC_ANALYZE "Enable MSVC static code analysis (/analyze)" OFF)
    if(ENABLE_MSVC_ANALYZE)
        add_compile_options(/analyze)
    endif()
else()
    add_compile_options(-Wall -Wextra)
endif()

if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
    if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
        set(CMAKE_BUILD_TYPE "Debug" CACHE STRING "Choose the type of build." FORCE)
        set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release" "RelWithDebInfo" "MinSizeRel")
    endif()

    option(UNIT_TEST "Unit Test Build" OFF)
    option(LEAK_CHECK "Memory Leak Detection" OFF)
    option(CUTIL_BUILD_EXAMPLE "Build Example Application" OFF)
endif()
include(CMakeDependentOption)

set(CUTIL_SANITIZERS "" CACHE STRING "Comma-separated sanitizers to enable (for example: address,undefined or thread)")
set_property(CACHE CUTIL_SANITIZERS PROPERTY STRINGS "" "address" "address,undefined" "thread")

if(CUTIL_SANITIZERS)
    if(CMAKE_C_COMPILER_ID MATCHES "GNU|Clang|AppleClang")
        add_compile_options(-fsanitize=${CUTIL_SANITIZERS} -fno-omit-frame-pointer)
        add_link_options(-fsanitize=${CUTIL_SANITIZERS})
    else()
        message(FATAL_ERROR "CUTIL_SANITIZERS is only supported with GCC or Clang")
    endif()
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
###

### Library cutil
set (CUTIL_HEADER_LIST
    ${CMAKE_CURRENT_SOURCE_DIR}/include/argparse.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/filestream.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/fileutil.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/pack.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/sha256.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/soa_chunk.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/soa_fsa.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/soa.h
)

set (CUTIL_SOURCE_LIST
    ${CMAKE_CURRENT_SOURCE_DIR}/src/argparse.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/filestream.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/fileutil.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/pack.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/sha256.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/soa_chunk.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/soa_fsa.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/soa.c
)

if (LEAK_CHECK)
    list (APPEND CUTIL_HEADER_LIST ${CMAKE_CURRENT_SOURCE_DIR}/include/CMemLeak.h)
    list (APPEND CUTIL_SOURCE_LIST ${CMAKE_CURRENT_SOURCE_DIR}/src/CMemLeak.c)
endif()

if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
    if(NOT TARGET adt)
        if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/../adt/CMakeLists.txt")
            add_subdirectory(../adt ${CMAKE_CURRENT_BINARY_DIR}/adt)
        else()
            message(FATAL_ERROR "adt submodule not found at ../adt")
        endif()
    endif()
endif()

add_library(cutil ${CUTIL_SOURCE_LIST} ${CUTIL_HEADER_LIST})
target_link_libraries(cutil PUBLIC adt)
target_include_directories(cutil PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
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
    if (UNIT_TEST)
        set (CUTIL_TEST_SUITE_LIST
            test/testsuite_argparse.c
            test/testsuite_filestream.c
            test/testsuite_fileutil.c
            test/testsuite_pack.c
            test/testsuite_sha256.c
            test/testsuite_soa_fsa.c
            test/testsuite_soa.c
        )

        add_executable(cutil_unit test/test_main.c ${CUTIL_TEST_SUITE_LIST})
        target_link_libraries(cutil_unit PRIVATE adt cutil cutest)
        target_include_directories(cutil_unit PRIVATE
                                  "${PROJECT_BINARY_DIR}"
                                  "${CMAKE_CURRENT_SOURCE_DIR}/include"
                                  "${CMAKE_CURRENT_SOURCE_DIR}/test"
                                  )
        target_compile_definitions(cutil_unit PRIVATE UNIT_TEST)

        if (LEAK_CHECK)
            target_compile_definitions(cutil_unit PRIVATE MEM_LEAK_CHECK)
        endif()

        enable_testing()
        add_test(NAME cutil_unit COMMAND cutil_unit)
        set_tests_properties(cutil_unit PROPERTIES
            PASS_REGULAR_EXPRESSION "OK \\([0-9]+ tests\\)"
            LABELS "unit"
        )
    endif()

    if (CUTIL_BUILD_EXAMPLE)
        set (CUTIL_EXAMPLE_SOURCES
            example/example_main.c
            example/argparse_example.c
            example/filestream_example.c
            example/fileutil_example.c
        )

        add_executable(cutil_example ${CUTIL_EXAMPLE_SOURCES})
        target_link_libraries(cutil_example PRIVATE cutil adt)
        target_include_directories(cutil_example PRIVATE
            "${CMAKE_CURRENT_SOURCE_DIR}/include"
            "${CMAKE_CURRENT_SOURCE_DIR}/example"
        )
    endif()
endif()
###


