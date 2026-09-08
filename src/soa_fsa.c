/*****************************************************************************
* \file      soa_fsa.c
* \author    Conny Gustafsson
* \date      2011-08-20
* \brief     Fixed-size block allocator
*
* Copyright (c) 2011-2026 Conny Gustafsson
* SPDX-License-Identifier: MIT
* See LICENSE in project root for full license terms.
******************************************************************************/
//////////////////////////////////////////////////////////////////////////////
// INCLUDES
//////////////////////////////////////////////////////////////////////////////
#include "soa_fsa.h"
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

void cutil_soa_fsa_init(cutil_soa_fsa_t *allocator, size_t block_size, unsigned char num_blocks)
{
   allocator->block_size = block_size;
   allocator->num_blocks = num_blocks;
   allocator->alloc_chunk = NULL;
   allocator->dealloc_chunk = NULL;
   allocator->chunks_len = 0;
   allocator->chunks = NULL;
}

void cutil_soa_fsa_destroy(cutil_soa_fsa_t *allocator)
{
   size_t i;
   cutil_soa_chunk_t *p;
   if (allocator->chunks != NULL)
   {
      for (i = 0, p = allocator->chunks; i < allocator->chunks_len; i++, p++)
      {
         cutil_soa_chunk_destroy(p);
      }
      free(allocator->chunks);
      allocator->chunks = NULL;
   }
}

void *cutil_soa_fsa_alloc(cutil_soa_fsa_t *allocator)
{
   if ((allocator->alloc_chunk == NULL) || (allocator->alloc_chunk->free_blocks == 0)) // No free blocks in this chunk or no chunk available
   {
      // Linear search through all chunks to find a free block
      size_t i;
      cutil_soa_chunk_t *chunk;
      allocator->alloc_chunk = NULL; // Invalidate alloc_chunk
      for (i = 0, chunk = allocator->chunks; i < allocator->chunks_len; i++, chunk++)
      {
         if (chunk->free_blocks > 0) // Space available?
         {
            allocator->alloc_chunk = chunk;
            break;
         }
      }
      if (allocator->alloc_chunk == NULL) // We still have not found a chunk with a free block?
      {
         cutil_soa_chunk_t *ptr;
         // Grow chunk array by one
         allocator->chunks_len++;
         if (allocator->chunks == NULL)
         {
            ptr = (cutil_soa_chunk_t*) malloc(allocator->chunks_len * sizeof(cutil_soa_chunk_t));
         }
         else
         {
            ptr = (cutil_soa_chunk_t*) realloc(allocator->chunks, allocator->chunks_len * sizeof(cutil_soa_chunk_t));
         }

         if (ptr != NULL)
         {
            if (ptr != allocator->chunks)
            {
               // The memory has moved, all pointers into allocator->chunks must be invalidated
               allocator->alloc_chunk = NULL;
               allocator->dealloc_chunk = NULL;
            }
            allocator->chunks = ptr;
            chunk = allocator->chunks + allocator->chunks_len - 1; // Pointer to last chunk
            cutil_soa_chunk_init(chunk, allocator->block_size, allocator->num_blocks); // Call constructor on newly created chunk
            allocator->alloc_chunk = chunk;
         }
         else
         {
            return NULL;
         }
      }
   }
   assert(allocator->alloc_chunk != NULL);
   assert(allocator->alloc_chunk->free_blocks > 0);
   return cutil_soa_chunk_alloc(allocator->alloc_chunk, allocator->block_size);
}

void cutil_soa_fsa_free(cutil_soa_fsa_t *allocator, void *ptr)
{
   size_t chunk_size_bytes = allocator->block_size * allocator->num_blocks;
   unsigned char *a, *b; // [a..b] is a (memory) range, where a is the first byte, and b is the last byte
   unsigned char *p = (unsigned char*) ptr;

   if (allocator->dealloc_chunk != NULL) // Is this deallocation in the same chunk as last time?
   {
      a = allocator->dealloc_chunk->block_data;
      b = allocator->dealloc_chunk->block_data + chunk_size_bytes;
      if ((a <= p) && (p <= b)) // Does p fall in the range [a..b]?
      {
         // Do nothing (dealloc_chunk is valid)
      }
      else
      {
         allocator->dealloc_chunk = NULL; // Invalidate dealloc_chunk
      }
   }
   if (allocator->dealloc_chunk == NULL)
   {
      // Which chunk does p belong to? (linear search)
      size_t i;
      cutil_soa_chunk_t *chunk;
      for (i = 0, chunk = allocator->chunks; i < allocator->chunks_len; i++, chunk++)
      {
         a = chunk->block_data;
         b = chunk->block_data + chunk_size_bytes;
         if ((a <= p) && (p <= b)) // Does p fall in the range [a..b]?
         {
            // Found it!
            allocator->dealloc_chunk = chunk;
            break;
         }
      }
   }
   assert(allocator->dealloc_chunk != NULL); // If this fails it means that ptr did not originate from this allocator
   cutil_soa_chunk_free(allocator->dealloc_chunk, ptr, allocator->block_size);
}

//////////////////////////////////////////////////////////////////////////////
// PRIVATE FUNCTIONS
//////////////////////////////////////////////////////////////////////////////
