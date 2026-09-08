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
 * \brief Joins a directory path and a filename into a newly allocated adt_str_t.
 *
 * Handles directory separator concatenation cleanly without duplicating slashes.
 *
 * \param dir Directory path.
 * \param filename File name or sub-path.
 * \return Pointer to newly allocated adt_str_t, or NULL on memory allocation failure.
 */
adt_str_t *cutil_path_join(const char *dir, const char *filename);

#ifdef __cplusplus
}
#endif

#endif // CUTIL_FILEUTIL_H_
