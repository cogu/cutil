/*****************************************************************************
* \file      argparse_example.c
* \author    Conny Gustafsson
* \date      2026-09-08
* \brief     Demonstration of cutil argument parser
*
* Copyright (c) 2026 Conny Gustafsson
* SPDX-License-Identifier: MIT
* See LICENSE in project root for full license terms.
******************************************************************************/
//////////////////////////////////////////////////////////////////////////////
// INCLUDES
//////////////////////////////////////////////////////////////////////////////
#include <stdio.h>
#include <string.h>
#include "argparse.h"
#include "example.h"

//////////////////////////////////////////////////////////////////////////////
// PRIVATE FUNCTIONS
//////////////////////////////////////////////////////////////////////////////
static cutil_argparse_result_t example_callback(const char *short_name, const char *long_name, const char *value)
{
   if (short_name != NULL)
   {
      if (strcmp(short_name, "h") == 0)
      {
         printf("  [Option] -h (Help requested)\n");
         return ARGPARSE_SUCCESS;
      }
      if (strcmp(short_name, "v") == 0)
      {
         printf("  [Option] -v (Verbose enabled)\n");
         return ARGPARSE_SUCCESS;
      }
      if (strcmp(short_name, "p") == 0)
      {
         if (value == NULL)
         {
            return ARGPARSE_NEED_VALUE; // Expect value in next argument
         }
         printf("  [Option] -p with port value: %s\n", value);
         return ARGPARSE_SUCCESS;
      }
      printf("  [Unknown short option] -%s\n", short_name);
      return ARGPARSE_NAME_ERROR;
   }
   if (long_name != NULL)
   {
      if (strcmp(long_name, "verbose") == 0)
      {
         printf("  [Option] --verbose\n");
         return ARGPARSE_SUCCESS;
      }
      if (strcmp(long_name, "port") == 0)
      {
         if (value == NULL)
         {
            return ARGPARSE_NEED_VALUE;
         }
         printf("  [Option] --port with value: %s\n", value);
         return ARGPARSE_SUCCESS;
      }
      if (strcmp(long_name, "config") == 0)
      {
         if (value == NULL)
         {
            return ARGPARSE_NEED_VALUE;
         }
         printf("  [Option] --config with path: %s\n", value);
         return ARGPARSE_SUCCESS;
      }
      printf("  [Unknown long option] --%s\n", long_name);
      return ARGPARSE_NAME_ERROR;
   }
   if (value != NULL)
   {
      printf("  [Positional argument] %s\n", value);
      return ARGPARSE_SUCCESS;
   }
   return ARGPARSE_PARSE_ERROR;
}

//////////////////////////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
//////////////////////////////////////////////////////////////////////////////
void run_argparse_example(void)
{
   printf("=== Running argparse example ===\n");
   const char *fake_argv[] = {
      "app",
      "-v",
      "-p", "8080",
      "--config=settings.json",
      "--verbose",
      "input_file.txt",
      "output_file.txt"
   };
   int fake_argc = (int)(sizeof(fake_argv) / sizeof(fake_argv[0]));

   printf("Parsing simulated command line arguments:\n");
   for (int i = 0; i < fake_argc; i++)
   {
      printf("  argv[%d] = %s\n", i, fake_argv[i]);
   }
   printf("\nParser callback output:\n");

   cutil_argparse_result_t res = cutil_argparse_exec(fake_argc, fake_argv, example_callback);
   printf("Result code: %d (%s)\n\n", res, res == ARGPARSE_SUCCESS ? "SUCCESS" : "ERROR");
}
