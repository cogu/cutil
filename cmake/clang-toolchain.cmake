# Clang toolchain file with automatic version detection.
# Searches for clang-22, clang-21, and unversioned clang in order.
# Pass -DCUTIL_CLANG_VERSION=<major> (for example: 21 or 22) to pin a specific version.

set(CUTIL_CLANG_VERSION "" CACHE STRING "Pin a specific Clang major version (for example: 21 or 22); leave empty to auto-detect")

if(NOT DEFINED CMAKE_C_COMPILER OR CMAKE_C_COMPILER STREQUAL "")
    if(CUTIL_CLANG_VERSION)
        set(_cutil_clang_candidates "clang-${CUTIL_CLANG_VERSION}")
    else()
        set(_cutil_clang_candidates clang-22 clang-21 clang)
    endif()

    find_program(_cutil_clang_c_compiler NAMES ${_cutil_clang_candidates})
    if(NOT _cutil_clang_c_compiler)
        if(CUTIL_CLANG_VERSION)
            message(FATAL_ERROR "Requested compiler 'clang-${CUTIL_CLANG_VERSION}' was not found in PATH.")
        else()
            message(FATAL_ERROR "No suitable Clang compiler found in PATH (searched: ${_cutil_clang_candidates}).")
        endif()
    endif()

    set(CMAKE_C_COMPILER "${_cutil_clang_c_compiler}" CACHE FILEPATH "C compiler" FORCE)
endif()
