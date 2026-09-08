/*****************************************************************************
* \file      test_main.c
* \author    Conny Gustafsson
* \date      2013-05-18
* \brief     Unit test entry point
*
* Copyright (c) 2013-2026 Conny Gustafsson
* SPDX-License-Identifier: MIT
* See LICENSE in project root for full license terms.
******************************************************************************/
//////////////////////////////////////////////////////////////////////////////
// INCLUDES
//////////////////////////////////////////////////////////////////////////////
#include <stdio.h>
#include <stdlib.h>
#include "CuTest.h"
#ifdef MEM_LEAK_CHECK
#include "CMemLeak.h"
#endif

//////////////////////////////////////////////////////////////////////////////
// PRIVATE CONSTANTS AND DATA TYPES
//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
// PRIVATE FUNCTION PROTOTYPES
//////////////////////////////////////////////////////////////////////////////
static void run_all_tests(void);

CuSuite* testsuite_pack(void);
CuSuite* testsuite_soa_fsa(void);
CuSuite* testsuite_soa(void);
CuSuite* testsuite_sha256(void);
CuSuite* testsuite_argparse(void);
CuSuite* testsuite_fileutil(void);
CuSuite* testsuite_filestream(void);

//////////////////////////////////////////////////////////////////////////////
// PRIVATE VARIABLES
//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
//////////////////////////////////////////////////////////////////////////////

int main(void)
{
   run_all_tests();
   return 0;
}

void vfree(void *arg)
{
   free(arg);
}

//////////////////////////////////////////////////////////////////////////////
// PRIVATE FUNCTIONS
//////////////////////////////////////////////////////////////////////////////

static void run_all_tests(void)
{
   CuString *output = CuStringNew();
   CuSuite* suite = CuSuiteNew();

   CuSuiteAddSuite(suite, testsuite_pack());
   CuSuiteAddSuite(suite, testsuite_soa_fsa());
   CuSuiteAddSuite(suite, testsuite_soa());
   CuSuiteAddSuite(suite, testsuite_sha256());
   CuSuiteAddSuite(suite, testsuite_argparse());
   CuSuiteAddSuite(suite, testsuite_fileutil());
   CuSuiteAddSuite(suite, testsuite_filestream());

   CuSuiteRun(suite);
   CuSuiteSummary(suite, output);
   CuSuiteDetails(suite, output);
   printf("%s\n", output->buffer);
   CuSuiteDelete(suite);
   CuStringDelete(output);
}
