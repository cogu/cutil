/*****************************************************************************
* \file      testsuite_soa_fsa.c
* \author    Conny Gustafsson
* \date      2019-02-25
* \brief     Unit tests for soa_fsa_t
*
* Copyright (c) 2019-2026 Conny Gustafsson
* SPDX-License-Identifier: MIT
* See LICENSE in project root for full license terms.
******************************************************************************/
//////////////////////////////////////////////////////////////////////////////
// INCLUDES
//////////////////////////////////////////////////////////////////////////////
#include <string.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include "CuTest.h"
#include "soa.h"
#ifdef MEM_LEAK_CHECK
#include "CMemLeak.h"
#endif

//////////////////////////////////////////////////////////////////////////////
// PRIVATE CONSTANTS AND DATA TYPES
//////////////////////////////////////////////////////////////////////////////
#define OVERFULL_BLOCK (SOA_DEFAULT_NUM_BLOCKS + 1)

//////////////////////////////////////////////////////////////////////////////
// PRIVATE FUNCTION PROTOTYPES
//////////////////////////////////////////////////////////////////////////////
static void test_alloc_256_1_byte_blocks(CuTest* tc);
static void test_alloc_500_1_byte_blocks(CuTest* tc);
static void test_alloc_1000_1_byte_blocks(CuTest* tc);
static void test_fill_one_chunk(CuTest* tc);
static void test_create_two_chunks(CuTest* tc);
static void test_free_3_at_beginning_then_allocate_5_more(CuTest* tc);

// Helper functions
static void do_1_byte_test(CuTest* tc, int32_t num_elements);
static bool check_if_already_allocated(void **array, int32_t array_len, void *ptr);

//////////////////////////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
//////////////////////////////////////////////////////////////////////////////

CuSuite* testsuite_soa_fsa(void)
{
   CuSuite* suite = CuSuiteNew();

   SUITE_ADD_TEST(suite, test_alloc_256_1_byte_blocks);
   SUITE_ADD_TEST(suite, test_alloc_500_1_byte_blocks);
   SUITE_ADD_TEST(suite, test_alloc_1000_1_byte_blocks);
   SUITE_ADD_TEST(suite, test_fill_one_chunk);
   SUITE_ADD_TEST(suite, test_create_two_chunks);
   SUITE_ADD_TEST(suite, test_free_3_at_beginning_then_allocate_5_more);

   return suite;
}

//////////////////////////////////////////////////////////////////////////////
// PRIVATE FUNCTIONS
//////////////////////////////////////////////////////////////////////////////

static void test_alloc_256_1_byte_blocks(CuTest* tc)
{
   const int32_t num_elements = 256;
   do_1_byte_test(tc, num_elements);
}

static void test_alloc_500_1_byte_blocks(CuTest* tc)
{
   const int32_t num_elements = 500;
   do_1_byte_test(tc, num_elements);
}

static void test_alloc_1000_1_byte_blocks(CuTest* tc)
{
   const int32_t num_elements = 1000;
   do_1_byte_test(tc, num_elements);
}

static void test_fill_one_chunk(CuTest* tc)
{
   cutil_soa_fsa_t fsa1;
   cutil_soa_fsa_init(&fsa1, sizeof(uint8_t), SOA_DEFAULT_NUM_BLOCKS);
   uint32_t i;
   void *ptr;

   void *allocated[SOA_DEFAULT_NUM_BLOCKS];

   memset(allocated, 0, sizeof(allocated));
   for (i = 0; i < SOA_DEFAULT_NUM_BLOCKS; i++)
   {
      ptr = cutil_soa_fsa_alloc(&fsa1);
      allocated[i] = ptr;
   }

   // After freeing one element in a full block, we expect the next alloc to return the previously freed pointer
   for (i = 0; i < SOA_DEFAULT_NUM_BLOCKS; i++)
   {
      void *previously_freed = allocated[i];
      cutil_soa_fsa_free(&fsa1, allocated[i]);
      ptr = cutil_soa_fsa_alloc(&fsa1);
      CuAssertPtrEquals(tc, previously_freed, ptr);
   }
   cutil_soa_fsa_destroy(&fsa1);
}

static void test_create_two_chunks(CuTest* tc)
{
   cutil_soa_fsa_t fsa1;
   uint32_t i;
   cutil_soa_chunk_t *chunk;
   void *ptr;

   cutil_soa_fsa_init(&fsa1, sizeof(uint8_t), SOA_DEFAULT_NUM_BLOCKS);

   void *allocated[OVERFULL_BLOCK];

   memset(allocated, 0, sizeof(allocated));
   for (i = 0; i < OVERFULL_BLOCK; i++)
   {
      ptr = cutil_soa_fsa_alloc(&fsa1);
      CuAssertPtrNotNull(tc, ptr);
      allocated[i] = ptr;
   }
   CuAssertIntEquals(tc, 2, (int) fsa1.chunks_len);
   chunk = &fsa1.chunks[0];
   for (i = 0; i < SOA_DEFAULT_NUM_BLOCKS; i++)
   {
      CuAssertTrue(tc, check_if_already_allocated(&allocated[0], 255, &chunk->block_data[i]));
   }
   chunk = &fsa1.chunks[1];
   CuAssertTrue(tc, check_if_already_allocated(&allocated[255], 1, &chunk->block_data[0]));
   cutil_soa_fsa_destroy(&fsa1);
}

static void test_free_3_at_beginning_then_allocate_5_more(CuTest* tc)
{
   cutil_soa_fsa_t fsa1;
   int32_t i;
   void *ptr;
   void *allocated1[255];
   void *allocated2[5];
   cutil_soa_fsa_init(&fsa1, sizeof(uint8_t), SOA_DEFAULT_NUM_BLOCKS);
   for (i = 0; i < 254; i++)
   {
      ptr = cutil_soa_fsa_alloc(&fsa1);
      CuAssertPtrNotNull(tc, ptr);
      allocated1[i] = ptr;
   }
   CuAssertIntEquals(tc, 1, (int) fsa1.chunks_len);
   CuAssertIntEquals(tc, 1, (int) fsa1.chunks[0].free_blocks);
   // Free 3 then allocate 5 more
   for (i = 0; i < 3; i++)
   {
      cutil_soa_fsa_free(&fsa1, allocated1[i]);
   }
   for (i = 0; i < 5; i++)
   {
      ptr = cutil_soa_fsa_alloc(&fsa1);
      CuAssertPtrNotNull(tc, ptr);
      allocated2[i] = ptr;
   }
   CuAssertIntEquals(tc, 2, (int) fsa1.chunks_len);
   CuAssertPtrEquals(tc, &fsa1.chunks[1].block_data[0], allocated2[4]);

   cutil_soa_fsa_destroy(&fsa1);
}

// Helper functions

static void do_1_byte_test(CuTest* tc, int32_t num_elements)
{
   cutil_soa_fsa_t fsa1;
   int32_t num_allocated;
   int32_t i;
   int32_t num_half = num_elements / 2;
   void** allocated = (void**) malloc(num_elements * sizeof(void*));
   CuAssertPtrNotNull(tc, allocated);
   memset(&allocated[0], 0, num_elements * sizeof(void*));
   cutil_soa_fsa_init(&fsa1, sizeof(uint8_t), SOA_DEFAULT_NUM_BLOCKS);
   for (num_allocated = 0; num_allocated < num_elements; num_allocated++)
   {
      char msg[100];
      void *ptr = cutil_soa_fsa_alloc(&fsa1);
      CuAssertPtrNotNull(tc, ptr);
      sprintf(msg, "(%p) Already exists, index=%d", ptr, (int) num_allocated);
      CuAssert(tc, msg, !check_if_already_allocated(&allocated[0], num_allocated, ptr));
      allocated[num_allocated] = ptr;
   }
   // Clear first half of the elements and allocate them again
   for (i = 0; i < num_half; i++)
   {
      cutil_soa_fsa_free(&fsa1, allocated[i]);
      allocated[i] = NULL;
   }
   // Check for duplicates (lower half)
   for (i = 0; i < num_half; i++)
   {
      char msg[100];
      void *ptr = cutil_soa_fsa_alloc(&fsa1);
      CuAssertPtrNotNull(tc, ptr);
      sprintf(msg, "(%p) Already exists, index=%d", ptr, (int) i);
      CuAssert(tc, msg, !check_if_already_allocated(&allocated[0], i, ptr));
      allocated[i] = ptr;
   }
   // Clear other half
   for (i = 0; i < num_half; i++)
   {
      cutil_soa_fsa_free(&fsa1, allocated[num_half + i]);
      allocated[num_half + i] = NULL;
   }
   // Check for duplicates (upper half)
   for (i = 0; i < num_half; i++)
   {
      char msg[100];
      void *ptr = cutil_soa_fsa_alloc(&fsa1);
      CuAssertPtrNotNull(tc, ptr);
      sprintf(msg, "(%p) Already exists, index=%d", ptr, (int) i);
      CuAssert(tc, msg, !check_if_already_allocated(&allocated[0], i, ptr));
      allocated[num_half + i] = ptr;
   }
   free(allocated);
   cutil_soa_fsa_destroy(&fsa1);
}

static bool check_if_already_allocated(void **array, int32_t array_len, void *ptr)
{
   int32_t i;
   for (i = 0; i < array_len; i++)
   {
      if (array[i] == ptr)
      {
         return true;
      }
   }
   return false;
}
