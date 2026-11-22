File and Path Utilities (fileutil)
==================================

The `fileutil` module provides platform-independent filesystem, path, atomic file I/O, and OS file locking utilities across Windows and POSIX operating systems.

Data Types
----------

.. doxygenstruct:: cutil_dir_entry_tag
   :members:

.. c:type:: cutil_dir_entry_cb

   Callback invoked for each entry during :c:func:`cutil_dir_for_each`:

   .. code-block:: c

      bool callback(void *arg, const cutil_dir_entry_t *entry);

   Return ``true`` to continue iterating, or ``false`` to stop early.

.. doxygenstruct:: cutil_file_lock_tag
   :members:

API Reference
-------------

Path and Existence Helpers
~~~~~~~~~~~~~~~~~~~~~~~~~~

.. doxygenfunction:: cutil_is_dir

.. doxygenfunction:: cutil_file_exists

.. doxygenfunction:: cutil_path_join

.. doxygenfunction:: cutil_path_append_extension

.. doxygenfunction:: cutil_path_replace_extension

Directory Operations
~~~~~~~~~~~~~~~~~~~~

.. doxygenfunction:: cutil_mkdir_p

.. doxygenfunction:: cutil_dir_for_each

File I/O and Durability
~~~~~~~~~~~~~~~~~~~~~~~

.. doxygenfunction:: cutil_read_binary_file

.. doxygenfunction:: cutil_write_binary_file

.. doxygenfunction:: cutil_read_regular_file

.. doxygenfunction:: cutil_file_replace_atomic

.. doxygenfunction:: cutil_file_touch_mtime

.. doxygenfunction:: cutil_file_set_mtime

.. doxygenfunction:: cutil_fsync_file

Exclusive File Locking
~~~~~~~~~~~~~~~~~~~~~~

.. doxygenfunction:: cutil_file_lock_init

.. doxygenfunction:: cutil_file_lock_try_acquire

.. doxygenfunction:: cutil_file_lock_release

.. doxygenfunction:: cutil_file_lock_is_held

Example
-------

The following example demonstrates directory validation and path concatenation:

.. literalinclude:: ../example/fileutil_example.c
   :language: c
   :lines: 22-42

