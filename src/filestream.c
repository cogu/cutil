/*****************************************************************************
* \file      filestream.c
* \author    Conny Gustafsson
* \date      2015-02-14
* \brief     File stream reader
*
* Copyright (c) 2015-2026 Conny Gustafsson
* SPDX-License-Identifier: MIT
* See LICENSE in project root for full license terms.
******************************************************************************/
//////////////////////////////////////////////////////////////////////////////
// INCLUDES
//////////////////////////////////////////////////////////////////////////////
#include <errno.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <stdio.h>
#include "filestream.h"
#ifdef MEM_LEAK_CHECK
#include "CMemLeak.h"
#endif

//////////////////////////////////////////////////////////////////////////////
// PRIVATE CONSTANTS AND DATA TYPES
//////////////////////////////////////////////////////////////////////////////
#define IFSTREAM_BLOCK_SIZE 8192

//////////////////////////////////////////////////////////////////////////////
// PRIVATE FUNCTION PROTOTYPES
//////////////////////////////////////////////////////////////////////////////
static void util_append_chunk(void *arg, const uint8_t *chunk, uint32_t chunk_len);

//////////////////////////////////////////////////////////////////////////////
// PRIVATE VARIABLES
//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
//////////////////////////////////////////////////////////////////////////////

void cutil_ifstream_create(cutil_ifstream_t *self, const cutil_ifstream_handler_t *handler)
{
   if (self != NULL && handler != NULL)
   {
      memcpy(&self->handler, handler, sizeof(cutil_ifstream_handler_t));
   }
}

void cutil_ifstream_destroy(cutil_ifstream_t *self)
{
   (void) self;
}

cutil_ifstream_t *cutil_ifstream_new(const cutil_ifstream_handler_t *handler)
{
   cutil_ifstream_t *self = (cutil_ifstream_t*) malloc(sizeof(cutil_ifstream_t));
   if (self != NULL)
   {
      cutil_ifstream_create(self, handler);
   }
   else
   {
      errno = ENOMEM;
   }
   return self;
}

void cutil_ifstream_delete(cutil_ifstream_t *self)
{
   if (self != NULL)
   {
      cutil_ifstream_destroy(self);
      free(self);
   }
}

void cutil_ifstream_open(cutil_ifstream_t *self)
{
   if ((self != NULL) && (self->handler.open != NULL))
   {
      self->handler.open(self->handler.arg);
   }
}

void cutil_ifstream_write(cutil_ifstream_t *self, const uint8_t *chunk, uint32_t chunk_len)
{
   if ((self != NULL) && (self->handler.write != NULL))
   {
      self->handler.write(self->handler.arg, chunk, chunk_len);
   }
}

void cutil_ifstream_close(cutil_ifstream_t *self)
{
   if ((self != NULL) && (self->handler.close != NULL))
   {
      self->handler.close(self->handler.arg);
   }
}

int cutil_ifstream_read_binary_file(cutil_ifstream_t *self, const char *filename)
{
   if ((self != NULL) && (filename != NULL))
   {
      uint32_t chunk_len = 0;
      char *chunk = (char*) malloc(IFSTREAM_BLOCK_SIZE);
      if (chunk != NULL)
      {
         FILE* fh = fopen(filename, "rb");
         if (fh != NULL)
         {
            if (self->handler.open != NULL)
            {
               self->handler.open(self->handler.arg);
            }
            while (1)
            {
               size_t len = fread(&chunk[0], 1, IFSTREAM_BLOCK_SIZE, fh);
               if (len > 0)
               {
                  chunk_len = (uint32_t) len;
                  if (self->handler.write != NULL)
                  {
                     self->handler.write(self->handler.arg, (uint8_t*) chunk, chunk_len);
                  }
                  if (len < IFSTREAM_BLOCK_SIZE)
                  {
                     break; //done
                  }
               }
               else
               {
                  break;
               }
            }
            if (self->handler.close != NULL)
            {
               self->handler.close(self->handler.arg);
            }
            free(chunk);
            fclose(fh);
            return 0;
         }
         free(chunk);
      }
   }
   else
   {
      errno = EINVAL;
   }
   return -1;
}

int cutil_ifstream_read_text_file(cutil_ifstream_t *self, const char *filename)
{
   int retval = 0;
   if ((self != NULL) && (filename != NULL))
   {
      FILE* fh = fopen(filename, "r");
      if (fh != NULL)
      {
         retval = cutil_ifstream_read_text_file_from_handle(self, fh);
         fclose(fh);
      }
      else
      {
         retval = -1;
      }
   }
   else
   {
      retval = -1;
      errno = EINVAL;
   }
   return retval;
}

int cutil_ifstream_read_text_file_from_handle(cutil_ifstream_t *self, FILE *fh)
{
   int retval = 0;
   if ((self != NULL) && (fh != NULL))
   {
      uint32_t chunk_len = 0;
      char *buf = (char*) malloc(IFSTREAM_BLOCK_SIZE);
      uint8_t *chunk = (uint8_t*) malloc(IFSTREAM_BLOCK_SIZE);
      if ((buf != NULL) && (chunk != NULL))
      {
         if (self->handler.open != NULL)
         {
            self->handler.open(self->handler.arg);
         }

         while (fgets(buf, IFSTREAM_BLOCK_SIZE, fh) != NULL)
         {
            size_t len = strlen(buf);
            assert(len > 0);
            //dos2unix file ending
            if ((len >= 2) && buf[len - 2] == '\r')
            {
               len--;
               buf[len - 1] = '\n';
            }
            if (len + chunk_len < IFSTREAM_BLOCK_SIZE)
            {
               memcpy(&chunk[chunk_len], buf, len); // NOLINT(bugprone-not-null-terminated-result)
               chunk_len += (uint32_t) len;
            }
            else
            {
               if (self->handler.write != NULL)
               {
                  self->handler.write(self->handler.arg, chunk, chunk_len);
               }
               memcpy(&chunk[0], buf, len); // NOLINT(bugprone-not-null-terminated-result)
               chunk_len = (uint32_t) len;
            }
         }
         if ((chunk_len > 0) && (self->handler.write != NULL))
         {
            self->handler.write(self->handler.arg, chunk, chunk_len);
         }
         if (self->handler.close != NULL)
         {
            self->handler.close(self->handler.arg);
         }
      }
      if (buf != NULL) free(buf);
      if (chunk != NULL) free(chunk);
   }
   else
   {
      errno = EINVAL;
      retval = -1;
   }
   return retval;
}

adt_bytearray_t *cutil_ifstream_util_read_text_file(const char *filename)
{
   adt_bytearray_t *buf = adt_bytearray_new();
   if (buf != NULL)
   {
      cutil_ifstream_handler_t handler;
      cutil_ifstream_t ifstream;
      memset(&handler, 0, sizeof(handler));
      handler.arg = buf;
      handler.write = util_append_chunk;
      cutil_ifstream_create(&ifstream, &handler);
      if (cutil_ifstream_read_text_file(&ifstream, filename) != 0)
      {
         adt_bytearray_delete(buf);
         buf = NULL;
      }
      return buf;
   }
   return NULL;
}

//////////////////////////////////////////////////////////////////////////////
// PRIVATE FUNCTIONS
//////////////////////////////////////////////////////////////////////////////

static void util_append_chunk(void *arg, const uint8_t *chunk, uint32_t chunk_len)
{
   adt_bytearray_t *buf = (adt_bytearray_t*) arg;
   if (buf != NULL && chunk != NULL && chunk_len > 0)
   {
      adt_bytearray_append(buf, chunk, chunk_len);
   }
}
