/*****************************************************************************
* \file      testsuite_soa.c
* \author    Conny Gustafsson
* \date      2026-09-08
* \brief     Unit tests for Small Object Allocator (cutil_soa_t)
*
* Copyright (c) 2026 Conny Gustafsson
* SPDX-License-Identifier: MIT
* See LICENSE in project root for full license terms.
******************************************************************************/
//////////////////////////////////////////////////////////////////////////////
// INCLUDES
//////////////////////////////////////////////////////////////////////////////
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "CuTest.h"
#include "soa.h"
#ifdef MEM_LEAK_CHECK
#include "CMemLeak.h"
#endif

//////////////////////////////////////////////////////////////////////////////
// PRIVATE CONSTANTS AND DATA TYPES
//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
// PRIVATE FUNCTION PROTOTYPES
//////////////////////////////////////////////////////////////////////////////
static void test_soa_init_and_destroy(CuTest *tc);
static void test_soa_alloc_and_free_small_objects(CuTest *tc);
static void test_soa_alloc_multiple_sizes(CuTest *tc);
static void test_soa_explicit_fsa_init(CuTest *tc);

//////////////////////////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
//////////////////////////////////////////////////////////////////////////////

CuSuite* testsuite_soa(void)
{
   CuSuite* suite = CuSuiteNew();

   SUITE_ADD_TEST(suite, test_soa_init_and_destroy);
   SUITE_ADD_TEST(suite, test_soa_alloc_and_free_small_objects);
   SUITE_ADD_TEST(suite, test_soa_alloc_multiple_sizes);
   SUITE_ADD_TEST(suite, test_soa_explicit_fsa_init);

   return suite;
}

//////////////////////////////////////////////////////////////////////////////
// PRIVATE FUNCTIONS
//////////////////////////////////////////////////////////////////////////////

static void test_soa_init_and_destroy(CuTest *tc)
{
   cutil_soa_t soa;
   cutil_soa_init(&soa);
   // Destroy empty allocator
   cutil_soa_destroy(&soa);
   CuAssertTrue(tc, true);
}

static void test_soa_alloc_and_free_small_objects(CuTest *tc)
{
   cutil_soa_t soa;
   cutil_soa_init(&soa);

   void *ptrs[100];
   int i;

   // Allocate 100 8-byte objects
   for (i = 0; i < 100; i++)
   {
      ptrs[i] = cutil_soa_alloc(&soa, 8);
      CuAssertPtrNotNull(tc, ptrs[i]);
      memset(ptrs[i], 0xAA, 8);
   }

   // Free all
   for (i = 0; i < 100; i++)
   {
      cutil_soa_free(&soa, ptrs[i], 8);
   }

   // Reallocate and verify they succeed
   for (i = 0; i < 100; i++)
   {
      ptrs[i] = cutil_soa_alloc(&soa, 8);
      CuAssertPtrNotNull(tc, ptrs[i]);
   }

   for (i = 0; i < 100; i++)
   {
      cutil_soa_free(&soa, ptrs[i], 8);
   }

   cutil_soa_destroy(&soa);
}

static void test_soa_alloc_multiple_sizes(CuTest *tc)
{
   cutil_soa_t soa;
   cutil_soa_init(&soa);

   void *p1 = cutil_soa_alloc(&soa, 1);
   void *p4 = cutil_soa_alloc(&soa, 4);
   void *p16 = cutil_soa_alloc(&soa, 16);
   void *p32 = cutil_soa_alloc(&soa, 32);

   CuAssertPtrNotNull(tc, p1);
   CuAssertPtrNotNull(tc, p4);
   CuAssertPtrNotNull(tc, p16);
   CuAssertPtrNotNull(tc, p32);

   memset(p1, 0x11, 1);
   memset(p4, 0x44, 4);
   memset(p16, 0x66, 16);
   memset(p32, 0x88, 32);

   cutil_soa_free(&soa, p1, 1);
   cutil_soa_free(&soa, p4, 4);
   cutil_soa_free(&soa, p16, 16);
   cutil_soa_free(&soa, p32, 32);

   cutil_soa_destroy(&soa);
}

static void test_soa_explicit_fsa_init(CuTest *tc)
{
   cutil_soa_t soa;
   cutil_soa_init(&soa);

   cutil_soa_init_fsa(&soa, 12, 16);
   void *ptr = cutil_soa_alloc(&soa, 12);
   CuAssertPtrNotNull(tc, ptr);
   cutil_soa_free(&soa, ptr, 12);

   cutil_soa_destroy(&soa);
}
