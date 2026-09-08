/*****************************************************************************
* \file      testsuite_pack.c
* \author    Conny Gustafsson
* \date      2013-05-18
* \brief     Unit tests for integer packing and unpacking routines
*
* Copyright (c) 2013-2026 Conny Gustafsson
* SPDX-License-Identifier: MIT
* See LICENSE in project root for full license terms.
******************************************************************************/
//////////////////////////////////////////////////////////////////////////////
// INCLUDES
//////////////////////////////////////////////////////////////////////////////
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <stddef.h>
#include <string.h>
#include "CuTest.h"
#include "pack.h"
#ifdef USE_PLATFORM_TYPES
#include "Platform_Types.h"
#endif
#ifdef MEM_LEAK_CHECK
#include "CMemLeak.h"
#endif

//////////////////////////////////////////////////////////////////////////////
// PRIVATE CONSTANTS AND DATA TYPES
//////////////////////////////////////////////////////////////////////////////
#define UINT64_SIZE 8

//////////////////////////////////////////////////////////////////////////////
// PRIVATE FUNCTION PROTOTYPES
//////////////////////////////////////////////////////////////////////////////
static void test_pack_unpack_le64(CuTest* tc);
static void test_pack_unpack_be_le(CuTest* tc);
static void test_unpack_little_endian_using_macros(CuTest* tc);
static void test_unpack_big_endian_using_macros(CuTest* tc);

//////////////////////////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
//////////////////////////////////////////////////////////////////////////////

CuSuite* testsuite_pack(void)
{
   CuSuite* suite = CuSuiteNew();

   SUITE_ADD_TEST(suite, test_pack_unpack_le64);
   SUITE_ADD_TEST(suite, test_pack_unpack_be_le);
   SUITE_ADD_TEST(suite, test_unpack_little_endian_using_macros);
   SUITE_ADD_TEST(suite, test_unpack_big_endian_using_macros);

   return suite;
}

//////////////////////////////////////////////////////////////////////////////
// PRIVATE FUNCTIONS
//////////////////////////////////////////////////////////////////////////////

static void test_pack_unpack_le64(CuTest* tc)
{
#ifdef USE_PLATFORM_TYPES
   uint64 value;
   uint64 result;
   uint8 buf[8];
#else
   uint64_t value;
   uint64_t result;
   uint8_t buf[8];
#endif
   uint32_t i;
   value = 0x0;
   cutil_pack_le64(&buf[0], value, UINT64_SIZE);
   for (i = 0; i < 8; i++)
   {
      CuAssertUIntEquals(tc, 0, buf[i]);
   }
   result = cutil_unpack_le64(&buf[0], UINT64_SIZE);
   CuAssertULIntEquals(tc, value, result);

   value = 0x0123456789ABCDEFUL;
   cutil_pack_le64(&buf[0], value, UINT64_SIZE);
   CuAssertUIntEquals(tc, 0xEF, buf[0]);
   CuAssertUIntEquals(tc, 0xCD, buf[1]);
   CuAssertUIntEquals(tc, 0xAB, buf[2]);
   CuAssertUIntEquals(tc, 0x89, buf[3]);
   CuAssertUIntEquals(tc, 0x67, buf[4]);
   CuAssertUIntEquals(tc, 0x45, buf[5]);
   CuAssertUIntEquals(tc, 0x23, buf[6]);
   CuAssertUIntEquals(tc, 0x01, buf[7]);
   result = cutil_unpack_le64(&buf[0], UINT64_SIZE);
   CuAssertULIntEquals(tc, value, result);

   value = 0xFFFFFFFFFFFFFFFFUL;
   cutil_pack_le64(&buf[0], value, UINT64_SIZE);
   for (i = 0; i < 8; i++)
   {
      CuAssertUIntEquals(tc, 0xFF, buf[i]);
   }
   result = cutil_unpack_le64(&buf[0], UINT64_SIZE);
   CuAssertULIntEquals(tc, value, result);
}

static void test_pack_unpack_be_le(CuTest* tc)
{
   uint8_t buf[4];
   uint32_t val = 0x12345678;

   // Test BE
   cutil_pack_be(buf, val, 4);
   CuAssertUIntEquals(tc, 0x12, buf[0]);
   CuAssertUIntEquals(tc, 0x34, buf[1]);
   CuAssertUIntEquals(tc, 0x56, buf[2]);
   CuAssertUIntEquals(tc, 0x78, buf[3]);
   CuAssertUIntEquals(tc, val, cutil_unpack_be(buf, 4));

   // Test LE
   cutil_pack_le(buf, val, 4);
   CuAssertUIntEquals(tc, 0x78, buf[0]);
   CuAssertUIntEquals(tc, 0x56, buf[1]);
   CuAssertUIntEquals(tc, 0x34, buf[2]);
   CuAssertUIntEquals(tc, 0x12, buf[3]);
   CuAssertUIntEquals(tc, val, cutil_unpack_le(buf, 4));
}

static void test_unpack_little_endian_using_macros(CuTest* tc)
{
#ifdef USE_PLATFORM_TYPES
   uint8 buf[4];
   uint8 u8_value;
   uint16 u16_value;
   uint32 u32_value;
#else
   uint8_t buf[4];
   uint8_t u8_value;
   uint16_t u16_value;
   uint32_t u32_value;
#endif
   uint32_t i;
   const uint8_t *p;
   const uint8_t *p_begin = &buf[0];
   for (i = 0; i < 4; i++)
   {
      buf[i] = (uint8_t) i;
   }
   p = p_begin;
   u8_value = cutil_unpack_u8(p);
   CuAssertConstPtrEquals(tc, p_begin + 1, p);
   CuAssertUIntEquals(tc, 0u, u8_value);
   p = p_begin;
   u16_value = cutil_unpack_u16_le(p);
   CuAssertConstPtrEquals(tc, p_begin + 2, p);
   CuAssertUIntEquals(tc, 1 * 256u, u16_value);
   p = p_begin;
   u32_value = cutil_unpack_u32_le(p);
   CuAssertConstPtrEquals(tc, p_begin + 4, p);
   CuAssertUIntEquals(tc, 1 * 256u + 2 * 65536u + 3 * 16777216u, u32_value);
}

static void test_unpack_big_endian_using_macros(CuTest* tc)
{
   uint8_t buf[4] = {0x12, 0x34, 0x56, 0x78};
   const uint8_t *p = buf;
   uint8_t u8_val;
   uint16_t u16_val;
   uint32_t u32_val;

   u8_val = cutil_unpack_u8(p);
   CuAssertConstPtrEquals(tc, buf + 1, p);
   CuAssertUIntEquals(tc, 0x12, u8_val);

   p = buf;
   u16_val = cutil_unpack_u16_be(p);
   CuAssertConstPtrEquals(tc, buf + 2, p);
   CuAssertUIntEquals(tc, 0x1234, u16_val);

   p = buf;
   u32_val = cutil_unpack_u32_be(p);
   CuAssertConstPtrEquals(tc, buf + 4, p);
   CuAssertUIntEquals(tc, 0x12345678, u32_val);
}
