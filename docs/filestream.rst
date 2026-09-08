File Stream Reader (filestream)
===============================

The `filestream` module provides chunk-based file streaming mechanisms inspired by stream processing patterns.

Data Types
----------

.. c:type:: cutil_ifstream_t

   Opaque structure wrapping a stream reader and its configured handler.

.. c:type:: cutil_ifstream_handler_t

   Handler callbacks structure containing:

   * ``void *arg``: User data passed to all callbacks.
   * ``void (*open)(void *arg)``: Callback invoked when the stream opens.
   * ``void (*close)(void *arg)``: Callback invoked when the stream closes.
   * ``void (*write)(void *arg, const uint8_t *chunk, uint32_t chunk_len)``: Callback invoked for each data chunk read.

API Reference
-------------

Lifecycle
~~~~~~~~~

.. doxygenfunction:: cutil_ifstream_create

.. doxygenfunction:: cutil_ifstream_destroy

.. doxygenfunction:: cutil_ifstream_new

.. doxygenfunction:: cutil_ifstream_delete

Stream Operations
~~~~~~~~~~~~~~~~~

.. doxygenfunction:: cutil_ifstream_open

.. doxygenfunction:: cutil_ifstream_write

.. doxygenfunction:: cutil_ifstream_close

Reading Files
~~~~~~~~~~~~~

.. doxygenfunction:: cutil_ifstream_read_binary_file

.. doxygenfunction:: cutil_ifstream_read_text_file

.. doxygenfunction:: cutil_ifstream_read_text_file_from_handle

.. doxygenfunction:: cutil_ifstream_util_read_text_file
