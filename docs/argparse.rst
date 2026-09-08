Argument Parser (argparse)
==========================

.. c:type:: cutil_argparse_result_t

   Enumeration representing argument parsing status codes:

   .. list-table::
      :header-rows: 1
      :widths: 35 15 50

      * - Constant
        - Value
        - Description
      * - ``ARGPARSE_INVALID_ARGUMENT_ERROR``
        - -5
        - NULL argument array or NULL callback pointer passed.
      * - ``ARGPARSE_PARSE_ERROR``
        - -4
        - Invalid syntax (e.g. ``---`` or multiple minus signs).
      * - ``ARGPARSE_MEM_ERROR``
        - -3
        - Memory allocation failure during string processing.
      * - ``ARGPARSE_VALUE_ERROR``
        - -2
        - Invalid value supplied for argument.
      * - ``ARGPARSE_NAME_ERROR``
        - -1
        - Unrecognized argument name.
      * - ``ARGPARSE_SUCCESS``
        - 0
        - Argument processed successfully.
      * - ``ARGPARSE_NEED_VALUE``
        - 1
        - Option expects an accompanying value in the next argument.

Overview
--------

The `argparse` module provides a callback-based parser for console arguments. It supports:

* Short options: ``-f``, ``-h``, ``-xvf``
* Long options: ``--version``, ``--target``, ``--help``
* Key-value arguments: ``--target=foo``, ``-p=5000``
* Positional arguments: ``filename.txt``, ``-`` (standard input/output)

Callback Function
-----------------

.. c:type:: cutil_argparse_callback_fn

   Callback invoked for each encountered argument:

   .. code-block:: c

      argparse_result_t callback(
          const char *short_name,
          const char *long_name,
          const char *value);

API Reference
-------------

.. doxygenfunction:: cutil_argparse_exec
