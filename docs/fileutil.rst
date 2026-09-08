File and Path Utilities (fileutil)
==================================

The `fileutil` module provides platform-independent filesystem utilities across Windows and POSIX operating systems.

API Reference
-------------

.. doxygenfunction:: cutil_is_dir

.. doxygenfunction:: cutil_path_join
 
Example
-------

The following example demonstrates directory validation and path concatenation:

.. literalinclude:: ../example/fileutil_example.c
   :language: c
   :lines: 22-42

