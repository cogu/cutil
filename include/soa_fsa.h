/*****************************************************************************
* \file      soa_fsa.h
* \author    Conny Gustafsson
* \date      2011-08-20
* \brief     Fixed-size block allocator
*
* Copyright (c) 2011-2026 Conny Gustafsson
* SPDX-License-Identifier: MIT
* See LICENSE in project root for full license terms.
******************************************************************************/
#ifndef CUTIL_SOA_FSA_H_
#define CUTIL_SOA_FSA_H_

//////////////////////////////////////////////////////////////////////////////
// INCLUDES
//////////////////////////////////////////////////////////////////////////////
#include "soa_chunk.h"

#ifdef __cplusplus
extern "C" {
#endif

//////////////////////////////////////////////////////////////////////////////
// PUBLIC CONSTANTS AND DATA TYPES
//////////////////////////////////////////////////////////////////////////////
typedef struct cutil_soa_fsa_tag
{
   size_t block_size;
   unsigned char num_blocks;
   cutil_soa_chunk_t *chunks;
   cutil_soa_chunk_t *alloc_chunk;
   cutil_soa_chunk_t *dealloc_chunk;
   size_t chunks_len;
} cutil_soa_fsa_t;

// Compatibility alias
typedef cutil_soa_fsa_t soa_fsa_t;

//////////////////////////////////////////////////////////////////////////////
// PUBLIC FUNCTION PROTOTYPES
//////////////////////////////////////////////////////////////////////////////

/**
 * \brief Initializes a fixed-size allocator for blocks of specified size.
 *
 * \param allocator Pointer to allocator structure.
 * \param block_size Size of each block in bytes.
 * \param num_blocks Number of blocks per chunk.
 */
void cutil_soa_fsa_init(cutil_soa_fsa_t *allocator, size_t block_size, unsigned char num_blocks);

/**
 * \brief Destroys a fixed-size allocator and frees all chunks.
 *
 * \param allocator Pointer to allocator structure.
 */
void cutil_soa_fsa_destroy(cutil_soa_fsa_t *allocator);

/**
 * \brief Allocates a single block from the fixed-size allocator.
 *
 * \param allocator Pointer to allocator structure.
 * \return Pointer to allocated block, or NULL on failure.
 */
void *cutil_soa_fsa_alloc(cutil_soa_fsa_t *allocator);

/**
 * \brief Frees a previously allocated block back into the allocator.
 *
 * \param allocator Pointer to allocator structure.
 * \param ptr Pointer to block to free.
 */
void cutil_soa_fsa_free(cutil_soa_fsa_t *allocator, void *ptr);

// Legacy function aliases
#define soa_fsa_init cutil_soa_fsa_init
#define soa_fsa_destroy cutil_soa_fsa_destroy
#define soa_fsa_alloc cutil_soa_fsa_alloc
#define soa_fsa_free cutil_soa_fsa_free

#ifdef __cplusplus
}
#endif

#endif // CUTIL_SOA_FSA_H_
