/*****************************************************************************
* \file      fileutil.h
* \author    Conny Gustafsson
* \date      2026-09-07
* \brief     Cross-platform file and path utilities
*
* Copyright (c) 2026 Conny Gustafsson
* SPDX-License-Identifier: MIT
* See LICENSE in project root for full license terms.
******************************************************************************/
#ifndef CUTIL_FILEUTIL_H_
#define CUTIL_FILEUTIL_H_

//////////////////////////////////////////////////////////////////////////////
// INCLUDES
//////////////////////////////////////////////////////////////////////////////
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "adt_str.h"

#ifdef __cplusplus
extern "C" {
#endif

//////////////////////////////////////////////////////////////////////////////
// PUBLIC CONSTANTS AND DATA TYPES
//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
// PUBLIC FUNCTION PROTOTYPES
//////////////////////////////////////////////////////////////////////////////

/**
 * \brief Checks if the given path is an existing directory.
 *
 * \param path File system path to check.
 * \return true if path exists and is a directory, false otherwise.
 */
bool cutil_is_dir(const char *path);

/**
 * \brief Checks if the given path is an existing regular file.
 *
 * \param path File system path to check.
 * \return true if path exists and is a regular file, false otherwise.
 */
bool cutil_file_exists(const char *path);

/**
 * \brief Joins a directory path and a filename into a newly allocated adt_str_t.
 *
 * Handles directory separator concatenation cleanly without duplicating slashes.
 *
 * \param dir Directory path.
 * \param filename File name or sub-path.
 * \return Pointer to newly allocated adt_str_t, or NULL on memory allocation failure.
 */
adt_str_t *cutil_path_join(const char *dir, const char *filename);

/**
 * \brief Appends an extension to a file path.
 *
 * For example, cutil_path_append_extension("node.apx", ".sig") returns "node.apx.sig".
 * If ext does not start with '.', one is automatically inserted.
 *
 * \param filepath Path string.
 * \param ext Extension to append.
 * \return Pointer to newly allocated adt_str_t, or NULL on error.
 */
adt_str_t *cutil_path_append_extension(const char *filepath, const char *ext);

/**
 * \brief Replaces or adds an extension on a file path.
 *
 * For example, cutil_path_replace_extension("node.apx", ".sig") returns "node.sig".
 *
 * \param filepath Path string.
 * \param new_ext New extension.
 * \return Pointer to newly allocated adt_str_t, or NULL on error.
 */
adt_str_t *cutil_path_replace_extension(const char *filepath, const char *new_ext);

/**
 * \brief Reads up to buf_size bytes from a binary file into a buffer.
 *
 * \param path Path to file.
 * \param buf Destination buffer.
 * \param buf_size Maximum bytes to read into buf.
 * \param bytes_read Optional pointer to receive actual count of bytes read.
 * \return 0 on success, -1 on error (e.g. file cannot be opened or path is NULL).
 */
int cutil_read_binary_file(const char *path, uint8_t *buf, size_t buf_size, size_t *bytes_read);

/**
 * \brief Writes a binary buffer to a file (creating or overwriting).
 *
 * \param path Path to file.
 * \param buf Data to write.
 * \param size Number of bytes to write.
 * \return 0 on success, -1 on error.
 */
int cutil_write_binary_file(const char *path, const uint8_t *buf, size_t size);

#ifdef __cplusplus
}
#endif

#endif // CUTIL_FILEUTIL_H_
