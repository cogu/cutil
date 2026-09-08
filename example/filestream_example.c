/*****************************************************************************
* \file      filestream_example.c
* \author    Conny Gustafsson
* \date      2026-09-08
* \brief     Demonstration of cutil filestream reader
*
* Copyright (c) 2026 Conny Gustafsson
* SPDX-License-Identifier: MIT
* See LICENSE in project root for full license terms.
******************************************************************************/
//////////////////////////////////////////////////////////////////////////////
// INCLUDES
//////////////////////////////////////////////////////////////////////////////
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "filestream.h"
#include "example.h"

//////////////////////////////////////////////////////////////////////////////
// PRIVATE CONSTANTS AND DATA TYPES
//////////////////////////////////////////////////////////////////////////////
typedef struct
{
   size_t chunk_count;
   size_t total_bytes;
} stream_stats_t;

//////////////////////////////////////////////////////////////////////////////
// PRIVATE FUNCTIONS
//////////////////////////////////////////////////////////////////////////////
static void on_stream_open(void *arg)
{
   stream_stats_t *stats = (stream_stats_t*) arg;
   printf("  [Stream opened]\n");
   stats->chunk_count = 0;
   stats->total_bytes = 0;
}

static void on_stream_write(void *arg, const uint8_t *chunk, uint32_t chunk_len)
{
   (void) chunk;
   stream_stats_t *stats = (stream_stats_t*) arg;
   stats->chunk_count++;
   stats->total_bytes += chunk_len;
   printf("  [Chunk %zu] Received %u bytes\n", stats->chunk_count, chunk_len);
}

static void on_stream_close(void *arg)
{
   stream_stats_t *stats = (stream_stats_t*) arg;
   printf("  [Stream closed] Processed %zu chunks, %zu total bytes\n",
          stats->chunk_count, stats->total_bytes);
}

//////////////////////////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
//////////////////////////////////////////////////////////////////////////////
void run_filestream_example(void)
{
   printf("=== Running filestream example ===\n");

   // 1. Streaming with custom handler
   stream_stats_t stats = {0, 0};
   cutil_ifstream_handler_t handler;
   memset(&handler, 0, sizeof(handler));
   handler.arg = &stats;
   handler.open = on_stream_open;
   handler.write = on_stream_write;
   handler.close = on_stream_close;

   cutil_ifstream_t stream;
   cutil_ifstream_create(&stream, &handler);

   const char *test_path = "CMakeLists.txt";
   printf("Streaming '%s' via cutil_ifstream_read_text_file:\n", test_path);
   int res = cutil_ifstream_read_text_file(&stream, test_path);
   printf("Stream result: %d (%s)\n\n", res, res == 0 ? "SUCCESS" : "ERROR");
   cutil_ifstream_destroy(&stream);

   // 2. Convenience utility to read entire text file into adt_bytearray_t
   printf("Reading first line using cutil_ifstream_util_read_text_file:\n");
   adt_bytearray_t *content = cutil_ifstream_util_read_text_file(test_path);
   if (content != NULL)
   {
      const char *data = (const char*) adt_bytearray_data(content);
      const char *newline = strchr(data, '\n');
      size_t first_line_len = (newline != NULL) ? (size_t)(newline - data) : strlen(data);
      printf("  First line: %.*s\n", (int) first_line_len, data);
      printf("  Total length: %u bytes\n\n", adt_bytearray_length(content));
      adt_bytearray_delete(content);
   }
}
