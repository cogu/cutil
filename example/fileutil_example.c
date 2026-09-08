/*****************************************************************************
* \file      fileutil_example.c
* \author    Conny Gustafsson
* \date      2026-09-08
* \brief     Demonstration of cutil path and filesystem utilities
*
* Copyright (c) 2026 Conny Gustafsson
* SPDX-License-Identifier: MIT
* See LICENSE in project root for full license terms.
******************************************************************************/
//////////////////////////////////////////////////////////////////////////////
// INCLUDES
//////////////////////////////////////////////////////////////////////////////
#include <stdio.h>
#include <stdbool.h>
#include "fileutil.h"
#include "example.h"

//////////////////////////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
//////////////////////////////////////////////////////////////////////////////
void run_fileutil_example(void)
{
   printf("=== Running fileutil example ===\n");

   // 1. Directory checking
   const char *dir_to_check = "include";
   const char *file_to_check = "CMakeLists.txt";

   printf("cutil_is_dir(\"%s\") = %s\n", dir_to_check, cutil_is_dir(dir_to_check) ? "true" : "false");
   printf("cutil_is_dir(\"%s\") = %s\n", file_to_check, cutil_is_dir(file_to_check) ? "true" : "false");

   // 2. Path joining
   const char *dir = "include";
   const char *filename = "argparse.h";
   adt_str_t *joined_path = cutil_path_join(dir, filename);
   if (joined_path != NULL)
   {
      printf("cutil_path_join(\"%s\", \"%s\") = %s\n\n",
             dir, filename, adt_str_cstr(joined_path));
      adt_str_delete(joined_path);
   }
}
