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
#include <stdio.h>
#include "adt_str.h"

#ifdef __cplusplus
extern "C" {
#endif

//////////////////////////////////////////////////////////////////////////////
// PUBLIC CONSTANTS AND DATA TYPES
//////////////////////////////////////////////////////////////////////////////
typedef struct cutil_dir_entry_tag
{
   const char *name;
   uint64_t size;
   int64_t mtime_sec;
   bool is_reg;
   bool is_dir;
   bool is_symlink;
} cutil_dir_entry_t;

typedef bool (cutil_dir_entry_cb)(void *arg, const cutil_dir_entry_t *entry);

typedef struct cutil_file_lock_tag
{
#ifdef _WIN32
   void *handle;
#else
   int fd;
#endif
   bool locked;
} cutil_file_lock_t;

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

/**
 * \brief Recursively creates directories for the given path (like `mkdir -p`).
 *
 * \param path Directory path to create.
 * \return 0 if the directory was created or already exists, -1 on error (preserving errno).
 */
int cutil_mkdir_p(const char *path);

/**
 * \brief Iterates over all entries in a directory (excluding "." and "..").
 *
 * Does not follow symbolic links when inspecting entry metadata.
 * Returning false from the callback stops iteration early with return code 0.
 *
 * \param dir_path Path to the directory.
 * \param cb Callback invoked for each entry.
 * \param arg User context passed as the first argument to cb.
 * \return 0 on success, -1 on error (preserving errno).
 */
int cutil_dir_for_each(const char *dir_path, cutil_dir_entry_cb cb, void *arg);

/**
 * \brief Reads an exact number of bytes from a regular file without following symlinks.
 *
 * \param path Path to the file.
 * \param buf Destination buffer of at least expected_size bytes (may be NULL when expected_size == 0).
 * \param expected_size Exact expected byte size of the file.
 * \param out_actual_size Optional output pointer for the actual regular file size.
 * \param out_mtime_sec Optional output pointer for the file's modification time (Unix epoch seconds).
 * \return 0 on exact match and full read;
 *         -2 if the file is a regular file whose size differs from expected_size;
 *         -1 on open, symlink, non-regular file, or I/O error (preserving errno).
 */
int cutil_read_regular_file(
   const char *path, uint8_t *buf, size_t expected_size, size_t *out_actual_size, int64_t *out_mtime_sec);

/**
 * \brief Atomically replaces dst_path with src_path (must reside on the same filesystem).
 *
 * \param src_path Source file path.
 * \param dst_path Destination file path.
 * \return 0 on success, -1 on error (preserving errno).
 */
int cutil_file_replace_atomic(const char *src_path, const char *dst_path);

/**
 * \brief Updates a file's modification time to the current time without following symlinks.
 *
 * \param path File path.
 * \return 0 on success, -1 on error (preserving errno).
 */
int cutil_file_touch_mtime(const char *path);

/**
 * \brief Sets a file's modification time to the given Unix epoch seconds without following symlinks.
 *
 * \param path File path.
 * \param mtime_sec Modification time in Unix epoch seconds.
 * \return 0 on success, -1 on error (preserving errno).
 */
int cutil_file_set_mtime(const char *path, int64_t mtime_sec);

/**
 * \brief Flushes C stdio buffers and syncs the underlying OS file descriptor to storage.
 *
 * \param fh Open FILE handle.
 * \return 0 on success, -1 on error (preserving errno).
 */
int cutil_fsync_file(FILE *fh);

/**
 * \brief Initializes a cutil_file_lock_t structure to the unlocked state.
 */
void cutil_file_lock_init(cutil_file_lock_t *lock);

/**
 * \brief Attempts to acquire an exclusive non-blocking OS lock on the given file path.
 *
 * Uses flock(LOCK_EX | LOCK_NB) on POSIX and LockFileEx(LOCKFILE_EXCLUSIVE_LOCK | LOCKFILE_FAIL_IMMEDIATELY)
 * on Windows. The OS kernel automatically releases the lock if the owning process exits or crashes.
 * On success, writes the current process ID into the lock file for diagnostics.
 *
 * \param lock Pointer to an initialized cutil_file_lock_t.
 * \param path Path to the lock file (created if it does not exist).
 * \return 0 if the exclusive lock was acquired;
 *         1 if the lock is already held by another handle/process;
 *        -1 on open or OS error (preserving errno).
 */
int cutil_file_lock_try_acquire(cutil_file_lock_t *lock, const char *path);

/**
 * \brief Releases a held OS file lock and closes its handle without unlinking the lock file.
 */
void cutil_file_lock_release(cutil_file_lock_t *lock);

/**
 * \brief Returns true if the lock handle currently holds an active lock.
 */
bool cutil_file_lock_is_held(const cutil_file_lock_t *lock);

#ifdef __cplusplus
}
#endif

#endif // CUTIL_FILEUTIL_H_
