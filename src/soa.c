/*****************************************************************************
* \file      soa.c
* \author    Conny Gustafsson
* \date      2011-08-20
* \brief     Small Object Allocator (An adaptation from "Modern C++ Design", chapter 4)
*
* Copyright (c) 2011-2026 Conny Gustafsson
* SPDX-License-Identifier: MIT
* See LICENSE in project root for full license terms.
******************************************************************************/
//////////////////////////////////////////////////////////////////////////////
// INCLUDES
//////////////////////////////////////////////////////////////////////////////
#include "soa.h"
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#ifdef MEM_LEAK_CHECK
#include "CMemLeak.h"
#endif

//////////////////////////////////////////////////////////////////////////////
// PRIVATE CONSTANTS AND DATA TYPES
//////////////////////////////////////////////////////////////////////////////
#define DEFAULT_NUM_BLOCKS 255
#define AUTO_INITIALIZE_FSA 1

//////////////////////////////////////////////////////////////////////////////
// PRIVATE FUNCTION PROTOTYPES
//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
// PRIVATE VARIABLES
//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
//////////////////////////////////////////////////////////////////////////////

void cutil_soa_init(cutil_soa_t *allocator)
{
   memset(allocator->fsa, 0, sizeof(cutil_soa_fsa_t*) * SOA_SMALL_OBJECT_MAX_SIZE);
}

void cutil_soa_destroy(cutil_soa_t *allocator)
{
   size_t i;
   for (i = 0; i < SOA_SMALL_OBJECT_MAX_SIZE; i++)
   {
      if (allocator->fsa[i] != NULL)
      {
         cutil_soa_fsa_destroy(allocator->fsa[i]);
         free(allocator->fsa[i]);
         allocator->fsa[i] = NULL;
      }
   }
}

void cutil_soa_init_fsa(cutil_soa_t *allocator, size_t block_size, unsigned char num_blocks)
{
   assert((block_size <= SOA_SMALL_OBJECT_MAX_SIZE) && (block_size > 0));
   if (allocator->fsa[block_size - 1] == NULL)
   {
      cutil_soa_fsa_t *ptr = (cutil_soa_fsa_t*) malloc(sizeof(cutil_soa_fsa_t));
      if (ptr != NULL)
      {
         cutil_soa_fsa_init(ptr, block_size, num_blocks);
         allocator->fsa[block_size - 1] = ptr;
      }
   }
}

void *cutil_soa_alloc(cutil_soa_t *allocator, size_t size)
{
   assert((size <= SOA_SMALL_OBJECT_MAX_SIZE) && (size > 0));
#if (AUTO_INITIALIZE_FSA)
   if (allocator->fsa[size - 1] == NULL)
   {
      cutil_soa_init_fsa(allocator, size, DEFAULT_NUM_BLOCKS);
   }
#endif

   assert(allocator->fsa[size - 1] != NULL);
   return cutil_soa_fsa_alloc(allocator->fsa[size - 1]);
}

void cutil_soa_free(cutil_soa_t *allocator, void *ptr, size_t size)
{
   assert((size <= SOA_SMALL_OBJECT_MAX_SIZE) && (size > 0));
   assert(allocator->fsa[size - 1] != NULL);
   cutil_soa_fsa_free(allocator->fsa[size - 1], ptr);
}

//////////////////////////////////////////////////////////////////////////////
// PRIVATE FUNCTIONS
//////////////////////////////////////////////////////////////////////////////
