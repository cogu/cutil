Integer Serialization & Packing (pack)
======================================

The `pack` module provides portable serialization and deserialization between native C integers and byte streams in both Big-Endian (Network Byte Order) and Little-Endian byte orders.

Byte Order Configuration
------------------------

When configured with ``-DBYTE_ORDER=AUTO`` (or ``LITTLE`` / ``BIG``), CMake detects platform endianness and accelerates compatible serialization routines using native memory operations. Setting ``-DBYTE_ORDER=NONE`` compiles platform-neutral shift-and-mask algorithms.

API Reference
-------------

32-bit Packing Functions
~~~~~~~~~~~~~~~~~~~~~~~~

.. doxygenfunction:: cutil_pack_be

.. doxygenfunction:: cutil_pack_le

.. doxygenfunction:: cutil_unpack_be

.. doxygenfunction:: cutil_unpack_le

64-bit Packing Functions
~~~~~~~~~~~~~~~~~~~~~~~~

.. doxygenfunction:: cutil_pack_le64

.. doxygenfunction:: cutil_unpack_le64

Stream Macros
~~~~~~~~~~~~~

The module also provides convenience macros that advance the destination or source buffer pointer as values are serialized/deserialized:

* ``cutil_pack_u8(p, v)``: Writes 1 byte and advances ``p`` by 1.
* ``cutil_pack_u16_be(p, v)``: Writes 2 bytes (Big Endian) and advances ``p`` by 2.
* ``cutil_pack_u32_be(p, v)``: Writes 4 bytes (Big Endian) and advances ``p`` by 4.
* ``cutil_pack_u16_le(p, v)``: Writes 2 bytes (Little Endian) and advances ``p`` by 2.
* ``cutil_pack_u32_le(p, v)``: Writes 4 bytes (Little Endian) and advances ``p`` by 4.
* ``cutil_unpack_u8(p)``: Reads 1 byte and advances ``p`` by 1.
* ``cutil_unpack_u16_be(p)``: Reads 2 bytes (Big Endian) and advances ``p`` by 2.
* ``cutil_unpack_u32_be(p)``: Reads 4 bytes (Big Endian) and advances ``p`` by 4.
* ``cutil_unpack_u16_le(p)``: Reads 2 bytes (Little Endian) and advances ``p`` by 2.
* ``cutil_unpack_u32_le(p)``: Reads 4 bytes (Little Endian) and advances ``p`` by 4.
