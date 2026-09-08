/*****************************************************************************
* \file      example_main.c
* \author    Conny Gustafsson
* \date      2026-09-08
* \brief     Driver entry point for cutil examples
*
* Copyright (c) 2026 Conny Gustafsson
* SPDX-License-Identifier: MIT
* See LICENSE in project root for full license terms.
******************************************************************************/
//////////////////////////////////////////////////////////////////////////////
// INCLUDES
//////////////////////////////////////////////////////////////////////////////
#include <stdio.h>
#include "example.h"

//////////////////////////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
//////////////////////////////////////////////////////////////////////////////
int main(void)
{
   printf("=========================================\n");
   printf("       cutil Library Examples            \n");
   printf("=========================================\n\n");

   run_argparse_example();
   run_filestream_example();
   run_fileutil_example();

   printf("All examples completed successfully.\n");
   return 0;
}
