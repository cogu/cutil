#include <stdio.h>
#include <stdlib.h>
#include "CuTest.h"
#ifdef MEM_LEAK_CHECK
#include "CMemLeak.h"
#endif


CuSuite* testsuite_pack(void);
CuSuite* testsuite_soa_fsa(void);
CuSuite* testsuite_sha256(void);

void RunAllTests(void)
{
   CuString *output = CuStringNew();
   CuSuite* suite = CuSuiteNew();

   CuSuiteAddSuite(suite, testsuite_pack());
   CuSuiteAddSuite(suite, testsuite_soa_fsa());
   CuSuiteAddSuite(suite, testsuite_sha256());

   CuSuiteRun(suite);
   CuSuiteSummary(suite, output);
   CuSuiteDetails(suite, output);
   printf("%s\n", output->buffer);
   CuSuiteDelete(suite);
   CuStringDelete(output);

}

int main(void)
{
   RunAllTests();
   return 0;
}

void vfree(void *arg)
{
   free(arg);
}
