/*****************************************************************************
* \file      fileutil.c
* \author    Conny Gustafsson
* \date      2026-09-07
* \brief     Cross-platform file and path utilities implementation
*
* Copyright (c) 2026 Conny Gustafsson
* SPDX-License-Identifier: MIT
* See LICENSE in project root for full license terms.
******************************************************************************/
//////////////////////////////////////////////////////////////////////////////
// INCLUDES
//////////////////////////////////////////////////////////////////////////////
#include <sys/stat.h>
#include <string.h>
#include <stdio.h>
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
# ifndef S_ISREG
#  define S_ISREG(mode) (((mode) & _S_IFREG) != 0)
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

bool cutil_file_exists(const char *path)
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

   struct stat st;
   if (stat(path, &st) == 0)
   {
      if (S_ISREG(st.st_mode))
      {
         return true;
      }
   }
   return false;
}

adt_str_t *cutil_path_append_extension(const char *filepath, const char *ext)
{
   if (filepath == NULL)
   {
      return NULL;
   }
   adt_str_t *result = adt_str_new_cstr(filepath);
   if (result == NULL)
   {
      return NULL;
   }
   if (ext != NULL && *ext != '\0')
   {
      if (*ext != '.')
      {
         adt_str_push(result, '.');
      }
      adt_str_append_cstr(result, ext);
   }
   return result;
}

adt_str_t *cutil_path_replace_extension(const char *filepath, const char *new_ext)
{
   if (filepath == NULL)
   {
      return NULL;
   }
   const char *last_dot = strrchr(filepath, '.');
   const char *last_sep = strrchr(filepath, '/');
#ifdef _WIN32
   const char *last_bs = strrchr(filepath, '\\');
   if (last_bs != NULL && (last_sep == NULL || last_bs > last_sep))
   {
      last_sep = last_bs;
   }
#endif
   if (last_dot != NULL && (last_sep == NULL || last_dot > last_sep))
   {
      size_t base_len = (size_t)(last_dot - filepath);
      adt_str_t *result = adt_str_new_bstr((const uint8_t*)filepath, (const uint8_t*)filepath + base_len);
      if (result == NULL)
      {
         return NULL;
      }
      if (new_ext != NULL && *new_ext != '\0')
      {
         if (*new_ext != '.')
         {
            adt_str_push(result, '.');
         }
         adt_str_append_cstr(result, new_ext);
      }
      return result;
   }
   else
   {
      return cutil_path_append_extension(filepath, new_ext);
   }
}

int cutil_read_binary_file(const char *path, uint8_t *buf, size_t buf_size, size_t *bytes_read)
{
   if (path == NULL || buf == NULL || buf_size == 0)
   {
      return -1;
   }
   FILE *fh = fopen(path, "rb");
   if (fh == NULL)
   {
      return -1;
   }
   size_t n = fread(buf, 1, buf_size, fh);
   fclose(fh);
   if (bytes_read != NULL)
   {
      *bytes_read = n;
   }
   return 0;
}

int cutil_write_binary_file(const char *path, const uint8_t *buf, size_t size)
{
   if (path == NULL || (buf == NULL && size > 0))
   {
      return -1;
   }
   FILE *fh = fopen(path, "wb");
   if (fh == NULL)
   {
      return -1;
   }
   if (size > 0)
   {
      size_t written = fwrite(buf, 1, size, fh);
      fclose(fh);
      if (written != size)
      {
         return -1;
      }
   }
   else
   {
      fclose(fh);
   }
   return 0;
}
