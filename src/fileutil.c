/*****************************************************************************
* \file      fileutil.c
* \author    Conny Gustafsson
* \date      2026-09-07
* \brief     Cross-platform file and path utilities implementation
*
* Copyright (c) 2026 Conny Gustafsson
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
#include <sys/stat.h>
#include <string.h>
#include "fileutil.h"
#ifdef MEM_LEAK_CHECK
#include "CMemLeak.h"
#endif

//////////////////////////////////////////////////////////////////////////////
// PRIVATE CONSTANTS AND DATA TYPES
//////////////////////////////////////////////////////////////////////////////
#ifndef CUTIL_PATH_MAX
#define CUTIL_PATH_MAX 4096
#endif

#ifdef _WIN32
# ifndef S_ISDIR
#  define S_ISDIR(mode) (((mode) & _S_IFDIR) != 0)
# endif
#endif

//////////////////////////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
//////////////////////////////////////////////////////////////////////////////

bool cutil_is_dir(const char *path)
{
   if (path == NULL)
   {
      return false;
   }

   size_t len = strlen(path);
   if (len == 0 || len >= CUTIL_PATH_MAX)
   {
      return false;
   }

   char clean_path[CUTIL_PATH_MAX];
   memcpy(clean_path, path, len + 1);

   // Strip trailing slashes, but keep root like "/" or "C:\" / "C:/"
   while (len > 1 && (clean_path[len - 1] == '/' || clean_path[len - 1] == '\\'))
   {
#ifdef _WIN32
      if (len == 3 && clean_path[1] == ':')
      {
         break;
      }
#endif
      clean_path[len - 1] = '\0';
      len--;
   }

   struct stat st;
   if (stat(clean_path, &st) == 0)
   {
      if (S_ISDIR(st.st_mode))
      {
         return true;
      }
   }
   return false;
}

adt_str_t *cutil_path_join(const char *dir, const char *filename)
{
   if (dir == NULL && filename == NULL)
   {
      return adt_str_new();
   }
   if (dir == NULL)
   {
      return adt_str_new_cstr(filename);
   }
   if (filename == NULL)
   {
      return adt_str_new_cstr(dir);
   }

   adt_str_t *retval = adt_str_new_cstr(dir);
   if (retval == NULL)
   {
      return NULL;
   }

   size_t dir_len = strlen(dir);
   bool dir_has_sep = (dir_len > 0 && (dir[dir_len - 1] == '/' || dir[dir_len - 1] == '\\'));
   const char *fn_ptr = filename;

   if (dir_has_sep)
   {
      while (*fn_ptr == '/' || *fn_ptr == '\\')
      {
         fn_ptr++;
      }
   }
   else
   {
      if (dir_len > 0 && *fn_ptr != '\0' && *fn_ptr != '/' && *fn_ptr != '\\')
      {
         adt_str_push(retval, '/');
      }
   }

   if (*fn_ptr != '\0')
   {
      adt_str_append_cstr(retval, fn_ptr);
   }

   return retval;
}
