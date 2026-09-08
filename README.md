![unit tests](https://github.com/cogu/cutil/workflows/unit%20tests/badge.svg)
[![Documentation Status](https://readthedocs.org/projects/c-cutil/badge/?version=latest)](https://c-cutil.readthedocs.io/en/latest/?badge=latest)

# cutil

Utilities for C-based projects.

Online documentation and API reference: **[c-cutil.readthedocs.io](https://c-cutil.readthedocs.io/)**

## What is it?

**cutil** is a collection of platform-independent, modular utility libraries and helper modules for C programming (C99 and later). It provides essential building blocks used across various C projects:

* **Command Line Parsing**: Lightweight callback-driven command line option parser supporting short options, long options, and positional arguments.
* **Stream & File Reading**: Chunk-based file reader with cross-platform CRLF-to-LF line ending normalization.
* **Filesystem & Path Utilities**: Cross-platform path concatenation and directory checking.
* **Binary Serialization / Packing**: Big-endian and little-endian integer packing and unpacking routines with optional platform byte-order hardware acceleration.
* **Cryptography**: Public domain NIST SHA-256 calculation implementation.
* **Memory Allocation**: Efficient Small Object Allocator (SOA) based on Andrei Alexandrescu's design.
* **Testing & Diagnostic Tools**: Includes the CuTest unit test framework and CMemLeak leak detector.

### Available Components

| Component | Header | Category | Description |
|-----------|--------|----------|-------------|
| `argparse` | `argparse.h` | First Party | Callback-driven command line argument parser |
| `filestream` | `filestream.h` | First Party | Stream file reader with CRLF normalization |
| `fileutil` | `fileutil.h` | First Party | Cross-platform directory detection and path join |
| `pack` | `pack.h` | First Party | 32-bit and 64-bit integer serialization/deserialization |
| `soa` | `soa.h` | First Party | Small object allocator with fixed-size pools |
| `sha256` | `sha256.h` | Third Party | Public domain SHA-256 hash calculation |
| `CuTest` | `cutest/CuTest.h` | Third Party | Lightweight C unit test framework |
| `CMemLeak` | `include/CMemLeak.h` | Third Party | Memory leak detection utility |

## Where is it used?

* [cogu/bstr](https://github.com/cogu/bstr)
* [cogu/c-apx](https://github.com/cogu/c-apx)
* [cogu/dtl_json](https://github.com/cogu/dtl_json)

This repository is also a submodule of the [cogu/c-apx](https://github.com/cogu/c-apx) project.

## Dependencies

* [cogu/adt](https://github.com/cogu/adt) (v0.3.5 or later)

When building standalone unit tests, clone `adt` and `cutil` side by side:

```bash
cd ~/repo
git clone https://github.com/cogu/adt.git
git clone https://github.com/cogu/cutil.git
cd cutil
```

## Building with CMake

### Using CMake Presets (Clang 18 + Ninja)

```bash
# Run unit tests
cmake --preset clang-test
cmake --build --preset clang-test
ctest --preset clang-test

# Address and Undefined Behavior Sanitizers (ASan + UBSan)
cmake --preset clang-asan
cmake --build --preset clang-asan
ctest --preset clang-asan

# Static Analysis
cmake --preset clang-tidy
cmake --build --preset clang-tidy
```

### Manual CMake Workflows (Linux and Windows)

For Windows, open a "Native Tools Command Prompt for Visual Studio".

#### Configure

```sh
cmake -S . -B build -DUNIT_TEST=ON
```

#### Build

```sh
cmake --build build --target cutil_unit
```

#### Run Unit Tests

```sh
ctest --test-dir build --output-on-failure
```

### CMake Options

| CMake Option | Usage | Default | Description |
|---|---|---|---|
| `UNIT_TEST` | `-DUNIT_TEST=ON` | `OFF` | Enables building unit test executable (`cutil_unit`) |
| `LEAK_CHECK` | `-DLEAK_CHECK=ON` | `OFF` | Enables memory leak detection via CMemLeak |
| `CUTIL_SANITIZERS` | `-DCUTIL_SANITIZERS="address,undefined"` | `""` | Enables compiler sanitizers (GCC / Clang) |
| `BYTE_ORDER` | `-DBYTE_ORDER=[AUTO\|NONE\|LITTLE\|BIG]` | `AUTO` | Platform endianness for pack optimizations |
| `ENABLE_MSVC_ANALYZE` | `-DENABLE_MSVC_ANALYZE=ON` | `OFF` | Enables MSVC static code analysis (`/analyze`) |

When `BYTE_ORDER` is `AUTO`, CMake automatically detects host byte order and enables optimized memory operations. Setting `NONE` selects safe portable algorithms for any architecture.
