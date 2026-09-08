/*****************************************************************************
* \file      soa_chunk.h
* \author    Conny Gustafsson
* \date      2011-08-20
* \brief     Chunk allocator for small objects
*
* Copyright (c) 2011-2026 Conny Gustafsson
* SPDX-License-Identifier: MIT
* See LICENSE in project root for full license terms.
******************************************************************************/
#ifndef CUTIL_SOA_CHUNK_H_
#define CUTIL_SOA_CHUNK_H_

//////////////////////////////////////////////////////////////////////////////
// INCLUDES
//////////////////////////////////////////////////////////////////////////////
#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif

//////////////////////////////////////////////////////////////////////////////
// PUBLIC CONSTANTS AND DATA TYPES
//////////////////////////////////////////////////////////////////////////////
typedef struct cutil_soa_chunk_tag
{
   unsigned char *block_data;
   unsigned char first_block;
   unsigned char free_blocks;
} cutil_soa_chunk_t;

// Compatibility alias
typedef cutil_soa_chunk_t soa_chunk_t;

//////////////////////////////////////////////////////////////////////////////
// PUBLIC FUNCTION PROTOTYPES
//////////////////////////////////////////////////////////////////////////////

/**
 * \brief Initializes a chunk with a fixed block size and number of blocks.
 *
 * \param chunk Pointer to chunk structure.
 * \param block_size Size of each block in bytes.
 * \param num_blocks Number of blocks in the chunk.
 */
void cutil_soa_chunk_init(cutil_soa_chunk_t *chunk, size_t block_size, unsigned char num_blocks);

/**
 * \brief Destroys a chunk and releases its allocated block memory.
 *
 * \param chunk Pointer to chunk structure.
 */
void cutil_soa_chunk_destroy(cutil_soa_chunk_t *chunk);

/**
 * \brief Allocates a block from the chunk.
 *
 * \param chunk Pointer to chunk structure.
 * \param block_size Size of block in bytes.
 * \return Pointer to allocated block, or NULL if no blocks are free.
 */
void *cutil_soa_chunk_alloc(cutil_soa_chunk_t *chunk, size_t block_size);

/**
 * \brief Frees a previously allocated block back into the chunk.
 *
 * \param chunk Pointer to chunk structure.
 * \param p Pointer to block to free.
 * \param block_size Size of block in bytes.
 */
void cutil_soa_chunk_free(cutil_soa_chunk_t *chunk, void *p, size_t block_size);

// Legacy function aliases
#define soa_chunk_init cutil_soa_chunk_init
#define soa_chunk_destroy cutil_soa_chunk_destroy
#define soa_chunk_alloc cutil_soa_chunk_alloc
#define soa_chunk_free cutil_soa_chunk_free

#ifdef __cplusplus
}
#endif

#endif // CUTIL_SOA_CHUNK_H_
