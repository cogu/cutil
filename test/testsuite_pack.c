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
// CONSTANTS AND DATA TYPES
//////////////////////////////////////////////////////////////////////////////
#define UINT64_SIZE 8
//////////////////////////////////////////////////////////////////////////////
// LOCAL FUNCTION PROTOTYPES
//////////////////////////////////////////////////////////////////////////////
static void test_pack_unpack_LE64(CuTest* tc);
static void test_unpack_little_endian_using_macros(CuTest* tc);

//////////////////////////////////////////////////////////////////////////////
// GLOBAL VARIABLES
//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
// LOCAL VARIABLES
//////////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////////
// GLOBAL FUNCTIONS
//////////////////////////////////////////////////////////////////////////////


CuSuite* testsuite_pack(void)
{
   CuSuite* suite = CuSuiteNew();

   SUITE_ADD_TEST(suite, test_pack_unpack_LE64);
   SUITE_ADD_TEST(suite, test_unpack_little_endian_using_macros);

   return suite;
}

//////////////////////////////////////////////////////////////////////////////
// LOCAL FUNCTIONS
//////////////////////////////////////////////////////////////////////////////
static void test_pack_unpack_LE64(CuTest* tc)
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
   packLE64(&buf[0], value, UINT64_SIZE);
   for (i=0; i<8; i++)
   {
      CuAssertUIntEquals(tc, 0, buf[i]);
   }
   result = unpackLE64(&buf[0], UINT64_SIZE);
   CuAssertULIntEquals(tc, value, result);

   value = 0x0123456789ABCDEFUL;
   packLE64(&buf[0], value, UINT64_SIZE);
   CuAssertUIntEquals(tc, 0xEF, buf[0]);
   CuAssertUIntEquals(tc, 0xCD, buf[1]);
   CuAssertUIntEquals(tc, 0xAB, buf[2]);
   CuAssertUIntEquals(tc, 0x89, buf[3]);
   CuAssertUIntEquals(tc, 0x67, buf[4]);
   CuAssertUIntEquals(tc, 0x45, buf[5]);
   CuAssertUIntEquals(tc, 0x23, buf[6]);
   CuAssertUIntEquals(tc, 0x01, buf[7]);
   result = unpackLE64(&buf[0], UINT64_SIZE);
   CuAssertULIntEquals(tc, value, result);

   value = 0xFFFFFFFFFFFFFFFFUL;
   packLE64(&buf[0], value, UINT64_SIZE);
   for (i=0; i<8; i++)
   {
      CuAssertUIntEquals(tc, 0xFF, buf[i]);
   }
   result = unpackLE64(&buf[0], UINT64_SIZE);
   CuAssertULIntEquals(tc, value, result);
}

static void test_unpack_little_endian_using_macros(CuTest* tc)
{
#ifdef USE_PLATFORM_TYPES
   uint8 buf[4];
   uint8 u8Value;
   uint16 u16Value;
   uint32 u32Value;
#else
   uint8_t buf[4];
   uint8_t u8Value;
   uint16_t u16Value;
   uint32_t u32Value;
#endif
   uint32_t i;
   const uint8_t *p;
   const uint8_t *pBegin = &buf[0];
   for (i=0; i<4; i++)
   {
      buf[i] = i;
   }
   p = pBegin;
   u8Value = unpackU8(p);
   CuAssertConstPtrEquals(tc, pBegin+1, p);
   CuAssertUIntEquals(tc, 0u, u8Value);
   p = pBegin;
   u16Value = unpackU16LE(p);
   CuAssertConstPtrEquals(tc, pBegin+2, p);
   CuAssertUIntEquals(tc, 1*256u, u16Value);
   p = pBegin;
   u32Value = unpackU32LE(p);
   CuAssertConstPtrEquals(tc, pBegin+4, p);
   CuAssertUIntEquals(tc, 1*256u+2*65536u+3*16777216u, u32Value);
}
