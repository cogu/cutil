/*****************************************************************************
* \file      testsuite_sha256.c
* \author    Conny Gustafsson
* \date      2020-02-03
* \brief     Unit tests for sha256
*
* Copyright (c) 2020 Conny Gustafsson
* Permission is hereby granted, free of charge, to any person obtaining a copy of
* this software and associated documentation files (the "Software"), to deal in
* the Software without restriction, including without limitation the rights to
* use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
* the Software, and to permit persons to whom the Software is furnished to do so,
* subject to the following conditions:

* The above copyright notice and this permission notice shall be included in all
* copies or substantial portions of the Software.

* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
* FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
* COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
* IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
* CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*
******************************************************************************/
//////////////////////////////////////////////////////////////////////////////
// INCLUDES
//////////////////////////////////////////////////////////////////////////////
#include <string.h>
#include <stdio.h>
#include "CuTest.h"
#include "filestream.h"
#include "adt_bytearray.h"
#include "sha256.h"
#ifdef MEM_LEAK_CHECK
#include "CMemLeak.h"
#endif


//////////////////////////////////////////////////////////////////////////////
// CONSTANTS AND DATA TYPES
//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
// LOCAL FUNCTION PROTOTYPES
//////////////////////////////////////////////////////////////////////////////
static void test_calc_test1(CuTest* tc);

//////////////////////////////////////////////////////////////////////////////
// LOCAL VARIABLES
//////////////////////////////////////////////////////////////////////////////
static const char *m_definition1 =
"APX/1.2\n"
"N\"TestNode1\"\n"
"P\"WheelBasedVehicleSpeed\"S\n"
"P\"CabTiltLockWarning\"C(0,7)\n"
"P\"VehicleMode\"C(0,15)\n"
"R\"GearSelectionMode\"C(0,7)\n\n";

//////////////////////////////////////////////////////////////////////////////
// GLOBAL FUNCTIONS
//////////////////////////////////////////////////////////////////////////////


CuSuite* testsuite_sha256(void)
{
   CuSuite* suite = CuSuiteNew();

   SUITE_ADD_TEST(suite, test_calc_test1);

   return suite;
}

//////////////////////////////////////////////////////////////////////////////
// LOCAL FUNCTIONS
//////////////////////////////////////////////////////////////////////////////

static void test_calc_test1(CuTest* tc)
{
   uint8_t hash[32];
   size_t size = strlen(m_definition1);
   memset(hash, 0, sizeof(hash));
   sha256_calc(hash, (void*) &m_definition1[0], size);
   CuAssertUIntEquals(tc, 0xb2, hash[0]);
   CuAssertUIntEquals(tc, 0xb6, hash[1]);
   CuAssertUIntEquals(tc, 0xb1, hash[2]);
   CuAssertUIntEquals(tc, 0x36, hash[3]);
   CuAssertUIntEquals(tc, 0x37, hash[4]);
   CuAssertUIntEquals(tc, 0xe5, hash[5]);
   CuAssertUIntEquals(tc, 0xe2, hash[6]);
   CuAssertUIntEquals(tc, 0xed, hash[7]);
   CuAssertUIntEquals(tc, 0x14, hash[8]);
   CuAssertUIntEquals(tc, 0x3d, hash[9]);
   CuAssertUIntEquals(tc, 0x79, hash[10]);
   CuAssertUIntEquals(tc, 0x25, hash[11]);
   CuAssertUIntEquals(tc, 0xfc, hash[12]);
   CuAssertUIntEquals(tc, 0x6c, hash[13]);
   CuAssertUIntEquals(tc, 0x2f, hash[14]);
   CuAssertUIntEquals(tc, 0x44, hash[15]);
   CuAssertUIntEquals(tc, 0xd3, hash[16]);
   CuAssertUIntEquals(tc, 0x9f, hash[17]);
   CuAssertUIntEquals(tc, 0xba, hash[18]);
   CuAssertUIntEquals(tc, 0xbd, hash[19]);
   CuAssertUIntEquals(tc, 0x8c, hash[20]);
   CuAssertUIntEquals(tc, 0x22, hash[21]);
   CuAssertUIntEquals(tc, 0x2f, hash[22]);
   CuAssertUIntEquals(tc, 0x19, hash[23]);
   CuAssertUIntEquals(tc, 0x8f, hash[24]);
   CuAssertUIntEquals(tc, 0x2c, hash[25]);
   CuAssertUIntEquals(tc, 0x43, hash[26]);
   CuAssertUIntEquals(tc, 0x73, hash[27]);
   CuAssertUIntEquals(tc, 0x08, hash[28]);
   CuAssertUIntEquals(tc, 0x51, hash[29]);
   CuAssertUIntEquals(tc, 0x19, hash[30]);
   CuAssertUIntEquals(tc, 0xa7, hash[31]);
}

