/*****************************************************************************
* \file      soa_chunk.c
* \author    Conny Gustafsson
* \date      2011-08-20
* \brief     Chunk allocator for small objects
*
* Copyright (c) 2011-2026 Conny Gustafsson
* SPDX-License-Identifier: MIT
* See LICENSE in project root for full license terms.
******************************************************************************/
//////////////////////////////////////////////////////////////////////////////
// INCLUDES
//////////////////////////////////////////////////////////////////////////////
#include "soa_chunk.h"
#include <stdlib.h>
#include <assert.h>
#ifdef MEM_LEAK_CHECK
#include "CMemLeak.h"
#endif

//////////////////////////////////////////////////////////////////////////////
// PRIVATE CONSTANTS AND DATA TYPES
//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
// PRIVATE FUNCTION PROTOTYPES
//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
// PRIVATE VARIABLES
//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
//////////////////////////////////////////////////////////////////////////////

void cutil_soa_chunk_init(cutil_soa_chunk_t *chunk, size_t block_size, unsigned char num_blocks)
{
   unsigned char i;
   unsigned char *p;
   chunk->block_data = (unsigned char*) malloc(block_size * num_blocks);
   if (chunk->block_data == NULL)
   {
      return;
   }
   chunk->first_block = 0;
   chunk->free_blocks = num_blocks;
   for (i = 0, p = chunk->block_data; i < num_blocks; p += block_size)
   {
      *p = ++i;
   }
   assert(p == chunk->block_data + (block_size * num_blocks));
}

void cutil_soa_chunk_destroy(cutil_soa_chunk_t *chunk)
{
   free(chunk->block_data);
   chunk->block_data = NULL;
}

void *cutil_soa_chunk_alloc(cutil_soa_chunk_t *chunk, size_t block_size)
{
   unsigned char *p;
   if (chunk->free_blocks == 0)
   {
      return NULL;
   }
   p = chunk->block_data + (chunk->first_block * block_size);
   chunk->first_block = *p; // Index of next available block is stored in byte 0 of the free block
   chunk->free_blocks--;
   return (void*) p;
}

void cutil_soa_chunk_free(cutil_soa_chunk_t *chunk, void *p, size_t block_size)
{
   size_t p_offset;
   size_t new_first_available_block;
   unsigned char *p_char = ((unsigned char*) p);
   assert(p_char >= chunk->block_data); // Assert that p belongs to this chunk
   p_offset = p_char - chunk->block_data;
   assert(p_offset % block_size == 0); // Assert that p is aligned to the first byte of a block
   *p_char = chunk->first_block; // Store index of first available block in byte 0 of the freed block
   new_first_available_block = p_offset / block_size;
   assert(new_first_available_block * block_size == p_offset); // Check for truncation error
   assert(new_first_available_block < 256); // Check for index out of bounds error
   chunk->first_block = (unsigned char) new_first_available_block;
   chunk->free_blocks++;
}

//////////////////////////////////////////////////////////////////////////////
// PRIVATE FUNCTIONS
//////////////////////////////////////////////////////////////////////////////
