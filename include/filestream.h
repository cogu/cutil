/*****************************************************************************
* \file      filestream.h
* \author    Conny Gustafsson
* \date      2015-02-14
* \brief     File stream reader
*
* Copyright (c) 2015-2026 Conny Gustafsson
* SPDX-License-Identifier: MIT
* See LICENSE in project root for full license terms.
******************************************************************************/
#ifndef CUTIL_FILESTREAM_H_
#define CUTIL_FILESTREAM_H_

//////////////////////////////////////////////////////////////////////////////
// INCLUDES
//////////////////////////////////////////////////////////////////////////////
#include <stdint.h>
#include <stdio.h>
#include "adt_bytearray.h"

#ifdef __cplusplus
extern "C" {
#endif

//////////////////////////////////////////////////////////////////////////////
// PUBLIC CONSTANTS AND DATA TYPES
//////////////////////////////////////////////////////////////////////////////
typedef struct cutil_ifstream_handler_tag {
   void *arg;
   void (*open)(void *arg);
   void (*close)(void *arg);
   void (*write)(void *arg, const uint8_t *chunk, uint32_t chunk_len);
} cutil_ifstream_handler_t;

typedef struct cutil_ifstream_tag {
   cutil_ifstream_handler_t handler;
} cutil_ifstream_t;

// Compatibility aliases
typedef cutil_ifstream_handler_t ifstream_handler_t;
typedef cutil_ifstream_t ifstream_t;

//////////////////////////////////////////////////////////////////////////////
// PUBLIC FUNCTION PROTOTYPES
//////////////////////////////////////////////////////////////////////////////

/**
 * \brief Initializes a filestream instance in-place with a handler.
 *
 * \param self Pointer to filestream instance.
 * \param handler Pointer to handler containing callbacks.
 */
void cutil_ifstream_create(cutil_ifstream_t *self, const cutil_ifstream_handler_t *handler);

/**
 * \brief Destroys an in-place initialized filestream instance.
 *
 * \param self Pointer to filestream instance.
 */
void cutil_ifstream_destroy(cutil_ifstream_t *self);

/**
 * \brief Allocates and initializes a new filestream instance on heap.
 *
 * \param handler Pointer to handler containing callbacks.
 * \return Newly allocated filestream instance, or NULL on error.
 */
cutil_ifstream_t *cutil_ifstream_new(const cutil_ifstream_handler_t *handler);

/**
 * \brief Destroys and frees a heap-allocated filestream instance.
 *
 * \param self Pointer to filestream instance.
 */
void cutil_ifstream_delete(cutil_ifstream_t *self);

/**
 * \brief Invokes the open callback on the filestream handler.
 *
 * \param self Pointer to filestream instance.
 */
void cutil_ifstream_open(cutil_ifstream_t *self);

/**
 * \brief Invokes the write callback on the filestream handler with a data chunk.
 *
 * \param self Pointer to filestream instance.
 * \param chunk Data buffer pointer.
 * \param chunk_len Number of bytes in chunk.
 */
void cutil_ifstream_write(cutil_ifstream_t *self, const uint8_t *chunk, uint32_t chunk_len);

/**
 * \brief Invokes the close callback on the filestream handler.
 *
 * \param self Pointer to filestream instance.
 */
void cutil_ifstream_close(cutil_ifstream_t *self);

/**
 * \brief Reads a binary file in blocks and dispatches them to handler callbacks.
 *
 * \param self Pointer to filestream instance.
 * \param filename Path to file to read.
 * \return 0 on success, or -1 on error.
 */
int cutil_ifstream_read_binary_file(cutil_ifstream_t *self, const char *filename);

/**
 * \brief Reads a text file, normalizes CRLF line endings, and dispatches to handler callbacks.
 *
 * \param self Pointer to filestream instance.
 * \param filename Path to file to read.
 * \return 0 on success, or -1 on error.
 */
int cutil_ifstream_read_text_file(cutil_ifstream_t *self, const char *filename);

/**
 * \brief Reads a text file from an open FILE handle and dispatches to handler callbacks.
 *
 * \param self Pointer to filestream instance.
 * \param fh Open FILE handle.
 * \return 0 on success, or -1 on error.
 */
int cutil_ifstream_read_text_file_from_handle(cutil_ifstream_t *self, FILE *fh);

/**
 * \brief Convenience utility to read an entire text file into a newly created adt_bytearray_t.
 *
 * \param filename Path to file to read.
 * \return Pointer to newly allocated adt_bytearray_t with file contents, or NULL on error.
 */
adt_bytearray_t *cutil_ifstream_util_read_text_file(const char *filename);

// Legacy macro aliases
#define ifstream_create cutil_ifstream_create
#define ifstream_destroy cutil_ifstream_destroy
#define ifstream_new cutil_ifstream_new
#define ifstream_delete cutil_ifstream_delete
#define ifstream_open cutil_ifstream_open
#define ifstream_write cutil_ifstream_write
#define ifstream_close cutil_ifstream_close
#define ifstream_readBinaryFile cutil_ifstream_read_binary_file
#define ifstream_readTextFile cutil_ifstream_read_text_file
#define ifstream_readTextFileFromHandle cutil_ifstream_read_text_file_from_handle
#define ifstream_util_readTextFile cutil_ifstream_util_read_text_file

#ifdef __cplusplus
}
#endif

#endif // CUTIL_FILESTREAM_H_
