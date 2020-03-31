# cutil
Utilities for C-based projects.

## What is it?

A collection of shared components which I use in many of my C-based projects.

### CuTest (Third Party)

A customized version of the [CuTest](http://cutest.sourceforge.net/) unit test framework. Has separate license file.

Look to the source code and header to see what modifications I made.

### CMemLeak (Third Party)

A platform-independent and compiler-independent memory leak checker for C.

### Filestream (First Party)

A file stream reader inspired by stream reading mechanism in Node.js.

### Pack (First Party)

Platform-independent (byte) serialization and deserialization routines for integers.

### sha256 (Third Party)

SHA-256 calculation routine. I have adapted the unit tests for CuTest.

### SOA (First Party)

A Small Object Allocator (SOA). This is actually my own C port of the *small object allocator* described in the excellent book "Modern C++ Design" by Andrei Alexandrescu (2001).

## Where is it used?

* [cogu/bstr](https://github.com/cogu/bstr)
* [cogu/c-apx](https://github.com/cogu/c-apx)
* [cogu/dtl_json](https://github.com/cogu/dtl_json)

This repo is a submodule of the [cogu/c-apx](https://github.com/cogu/c-apx) (top-level) project.

## Dependencies

* [cogu/adt](https://github.com/cogu/adt)

The unit test project(s) assumes that the repos are cloned (separately) into a common directory as seen below.

* adt
* cutil (this repo)

### Example

```bash
$ cd ~
$ mkdir repo && cd repo
$ git clone https://github.com/cogu/adt.git
$ git clone https://github.com/cogu/cutil.git
$ cd cutil
```

## Building with CMake

CMake files exist but has so far only been tested on Linux.

First clone this repo and its dependencies into a common directory (such as ~/repo) as seen above. Alternatively the repos can be submodules of a top-level repo (as seen in [cogu/c-apx](https://github.com/cogu/c-apx)).

### Running unit tests (Linux)

```bash
$ mkdir UnitTest && cd UnitTest
$ cmake -DCMAKE_BUILD_TYPE=UnitTest ..
$ cmake --build .
$ ./cutil_unit
```
