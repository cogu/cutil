Small Object Allocator (soa)
============================

The `soa` module provides a Small Object Allocator adapted into C from Chapter 4 of Andrei Alexandrescu's book *Modern C++ Design* (2001).

Architecture
------------

The allocator is organized in a three-tier hierarchy:

1. **Chunk Allocator** (:c:type:`cutil_soa_chunk_t`):
   Manages a single contiguous buffer divided into fixed-size blocks (up to 255 blocks). It tracks free blocks using an embedded free-index list inside unallocated blocks.
2. **Fixed-Size Allocator** (:c:type:`cutil_soa_fsa_t`):
   Manages an array of chunks all dedicated to blocks of a specific byte size.
3. **Small Object Allocator** (:c:type:`cutil_soa_t`):
   Manages an array of fixed-size allocators, routing allocations for sizes from 1 up to ``SOA_SMALL_OBJECT_MAX_SIZE`` (32 bytes) to the appropriate fixed-size allocator.

Data Types
----------

.. c:type:: cutil_soa_t

   Top-level small object allocator structure containing an array of fixed-size allocators.

.. c:type:: cutil_soa_fsa_t

   Fixed-size allocator structure managing chunks of a single block size.

.. c:type:: cutil_soa_chunk_t

   Single memory chunk managing an array of uniform blocks.

API Reference
-------------

Small Object Allocator
~~~~~~~~~~~~~~~~~~~~~~

.. doxygenfunction:: cutil_soa_init

.. doxygenfunction:: cutil_soa_destroy

.. doxygenfunction:: cutil_soa_init_fsa

.. doxygenfunction:: cutil_soa_alloc

.. doxygenfunction:: cutil_soa_free

Fixed Size Allocator
~~~~~~~~~~~~~~~~~~~~

.. doxygenfunction:: cutil_soa_fsa_init

.. doxygenfunction:: cutil_soa_fsa_destroy

.. doxygenfunction:: cutil_soa_fsa_alloc

.. doxygenfunction:: cutil_soa_fsa_free

Chunk Allocator
~~~~~~~~~~~~~~~

.. doxygenfunction:: cutil_soa_chunk_init

.. doxygenfunction:: cutil_soa_chunk_destroy

.. doxygenfunction:: cutil_soa_chunk_alloc

.. doxygenfunction:: cutil_soa_chunk_free
