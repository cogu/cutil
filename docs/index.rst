C Utilities (CUTIL)
===================

**CUTIL** provides essential cross-platform and compiler-independent utilities for C programming projects (C99 and later).

It provides modular components for argument parsing, stream reading, cross-platform filesystem handling, integer serialization/packing, cryptography, and small object memory allocation.

.. toctree::
   :maxdepth: 2
   :hidden:
   :caption: API Reference

   argparse
   filestream
   fileutil
   pack
   sha256
   soa


Components Catalog
==================

Below is a summary of all modules provided by the CUTIL library:

.. list-table::
   :header-rows: 1
   :widths: 20 20 60

   * - Module
     - Header
     - Description
   * - :doc:`argparse`
     - ``argparse.h``
     - Lightweight callback-driven command line argument parser
   * - :doc:`filestream`
     - ``filestream.h``
     - Chunk-based file streaming reader with cross-platform CRLF normalization
   * - :doc:`fileutil`
     - ``fileutil.h``
     - Cross-platform filesystem and path utilities (directory detection, path joining)
   * - :doc:`pack`
     - ``pack.h``
     - Big-endian and little-endian binary serialization and deserialization routines
   * - :doc:`sha256`
     - ``sha256.h``
     - Standard SHA-256 cryptographic hash computation (public domain)
   * - :doc:`soa`
     - ``soa.h``
     - Fast Small Object Allocator adapted from Andrei Alexandrescu's design
