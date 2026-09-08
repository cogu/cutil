/*****************************************************************************
* \file      soa.h
* \author    Conny Gustafsson
* \date      2011-08-20
* \brief     Small Object Allocator (An adaptation from "Modern C++ Design", chapter 4)
*
* Copyright (c) 2011-2026 Conny Gustafsson
* SPDX-License-Identifier: MIT
* See LICENSE in project root for full license terms.
******************************************************************************/
#ifndef CUTIL_SOA_H_
#define CUTIL_SOA_H_

//////////////////////////////////////////////////////////////////////////////
// INCLUDES
//////////////////////////////////////////////////////////////////////////////
#include "soa_fsa.h"

#ifdef __cplusplus
extern "C" {
#endif

//////////////////////////////////////////////////////////////////////////////
// PUBLIC CONSTANTS AND DATA TYPES
//////////////////////////////////////////////////////////////////////////////
#define SOA_SMALL_OBJECT_MAX_SIZE 32 // Maximum size (in bytes) of object to be considered "small"
#define SOA_DEFAULT_NUM_BLOCKS 255u

typedef struct cutil_soa_tag
{
   cutil_soa_fsa_t *fsa[SOA_SMALL_OBJECT_MAX_SIZE];
} cutil_soa_t;

// Compatibility alias
typedef cutil_soa_t soa_t;

//////////////////////////////////////////////////////////////////////////////
// PUBLIC FUNCTION PROTOTYPES
//////////////////////////////////////////////////////////////////////////////

/**
 * \brief Initializes a small object allocator instance.
 *
 * \param allocator Pointer to allocator structure.
 */
void cutil_soa_init(cutil_soa_t *allocator);

/**
 * \brief Destroys a small object allocator instance and releases all fixed-size allocators.
 *
 * \param allocator Pointer to allocator structure.
 */
void cutil_soa_destroy(cutil_soa_t *allocator);

/**
 * \brief Initializes a fixed-size allocator for a specific block size.
 *
 * \param allocator Pointer to allocator structure.
 * \param block_size Block size to allocate for (1 to SOA_SMALL_OBJECT_MAX_SIZE).
 * \param num_blocks Number of blocks per chunk.
 */
void cutil_soa_init_fsa(cutil_soa_t *allocator, size_t block_size, unsigned char num_blocks);

/**
 * \brief Allocates memory of the specified size (<= SOA_SMALL_OBJECT_MAX_SIZE).
 *
 * \param allocator Pointer to allocator structure.
 * \param size Size in bytes.
 * \return Pointer to allocated block, or NULL on failure.
 */
void *cutil_soa_alloc(cutil_soa_t *allocator, size_t size);

/**
 * \brief Frees previously allocated memory back to the small object allocator.
 *
 * \param allocator Pointer to allocator structure.
 * \param ptr Pointer to block to free.
 * \param size Size of the block originally allocated.
 */
void cutil_soa_free(cutil_soa_t *allocator, void *ptr, size_t size);

// Legacy function aliases
#define soa_init cutil_soa_init
#define soa_destroy cutil_soa_destroy
#define soa_initFSA cutil_soa_init_fsa
#define soa_alloc cutil_soa_alloc
#define soa_free cutil_soa_free

#ifdef __cplusplus
}
#endif

#endif // CUTIL_SOA_H_
