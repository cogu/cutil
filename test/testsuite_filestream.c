/*****************************************************************************
* \file      testsuite_filestream.c
* \author    Conny Gustafsson
* \date      2026-09-08
* \brief     Unit tests for filestream
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
#include "CuTest.h"
#include "filestream.h"
#include "adt_bytearray.h"
#ifdef MEM_LEAK_CHECK
#include "CMemLeak.h"
#endif

//////////////////////////////////////////////////////////////////////////////
// PRIVATE CONSTANTS AND DATA TYPES
//////////////////////////////////////////////////////////////////////////////
typedef struct test_stream_sink_tag {
   adt_bytearray_t *buf;
   int open_count;
   int close_count;
} test_stream_sink_t;

//////////////////////////////////////////////////////////////////////////////
// PRIVATE FUNCTION PROTOTYPES
//////////////////////////////////////////////////////////////////////////////
static void test_filestream_create_and_destroy(CuTest *tc);
static void test_filestream_new_and_delete(CuTest *tc);
static void test_filestream_read_text_file(CuTest *tc);
static void test_filestream_read_binary_file(CuTest *tc);
static void test_filestream_util_read_text_file(CuTest *tc);

static void sink_open(void *arg);
static void sink_close(void *arg);
static void sink_write(void *arg, const uint8_t *chunk, uint32_t chunk_len);

//////////////////////////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
//////////////////////////////////////////////////////////////////////////////

CuSuite* testsuite_filestream(void)
{
   CuSuite* suite = CuSuiteNew();

   SUITE_ADD_TEST(suite, test_filestream_create_and_destroy);
   SUITE_ADD_TEST(suite, test_filestream_new_and_delete);
   SUITE_ADD_TEST(suite, test_filestream_read_text_file);
   SUITE_ADD_TEST(suite, test_filestream_read_binary_file);
   SUITE_ADD_TEST(suite, test_filestream_util_read_text_file);

   return suite;
}

//////////////////////////////////////////////////////////////////////////////
// PRIVATE FUNCTIONS
//////////////////////////////////////////////////////////////////////////////

static void sink_open(void *arg)
{
   test_stream_sink_t *sink = (test_stream_sink_t*) arg;
   sink->open_count++;
}

static void sink_close(void *arg)
{
   test_stream_sink_t *sink = (test_stream_sink_t*) arg;
   sink->close_count++;
}

static void sink_write(void *arg, const uint8_t *chunk, uint32_t chunk_len)
{
   test_stream_sink_t *sink = (test_stream_sink_t*) arg;
   if (sink->buf != NULL && chunk != NULL && chunk_len > 0)
   {
      adt_bytearray_append(sink->buf, chunk, chunk_len);
   }
}

static void test_filestream_create_and_destroy(CuTest *tc)
{
   cutil_ifstream_t stream;
   cutil_ifstream_handler_t handler;
   test_stream_sink_t sink;
   uint8_t data[] = {1, 2, 3, 4};

   sink.buf = adt_bytearray_new();
   CuAssertPtrNotNull(tc, sink.buf);
   sink.open_count = 0;
   sink.close_count = 0;

   memset(&handler, 0, sizeof(handler));
   handler.arg = &sink;
   handler.open = sink_open;
   handler.close = sink_close;
   handler.write = sink_write;

   cutil_ifstream_create(&stream, &handler);
   cutil_ifstream_open(&stream);
   CuAssertIntEquals(tc, 1, sink.open_count);

   cutil_ifstream_write(&stream, data, 4);
   CuAssertUIntEquals(tc, 4, adt_bytearray_length(sink.buf));

   cutil_ifstream_close(&stream);
   CuAssertIntEquals(tc, 1, sink.close_count);

   cutil_ifstream_destroy(&stream);
   adt_bytearray_delete(sink.buf);
}

static void test_filestream_new_and_delete(CuTest *tc)
{
   cutil_ifstream_handler_t handler;
   memset(&handler, 0, sizeof(handler));
   cutil_ifstream_t *stream = cutil_ifstream_new(&handler);
   CuAssertPtrNotNull(tc, stream);
   cutil_ifstream_delete(stream);
}

static void test_filestream_read_text_file(CuTest *tc)
{
   const char *test_filename = "test_filestream_tmp.txt";
   const char *content = "Hello World!\r\nSecond line.\r\nThird line.";
   FILE *fh = fopen(test_filename, "wb");
   CuAssertPtrNotNull(tc, fh);
   fputs(content, fh);
   fclose(fh);

   test_stream_sink_t sink;
   sink.buf = adt_bytearray_new();
   CuAssertPtrNotNull(tc, sink.buf);
   sink.open_count = 0;
   sink.close_count = 0;

   cutil_ifstream_handler_t handler;
   memset(&handler, 0, sizeof(handler));
   handler.arg = &sink;
   handler.open = sink_open;
   handler.close = sink_close;
   handler.write = sink_write;

   cutil_ifstream_t stream;
   cutil_ifstream_create(&stream, &handler);

   int rc = cutil_ifstream_read_text_file(&stream, test_filename);
   CuAssertIntEquals(tc, 0, rc);
   CuAssertIntEquals(tc, 1, sink.open_count);
   CuAssertIntEquals(tc, 1, sink.close_count);

   // Text mode converts CRLF to LF
   const char *expected = "Hello World!\nSecond line.\nThird line.";
   const uint8_t *result_data = adt_bytearray_data(sink.buf);
   uint32_t result_len = adt_bytearray_length(sink.buf);
   CuAssertUIntEquals(tc, (uint32_t) strlen(expected), result_len);
   CuAssertIntEquals(tc, 0, memcmp(expected, result_data, result_len));

   cutil_ifstream_destroy(&stream);
   adt_bytearray_delete(sink.buf);
   remove(test_filename);
}

static void test_filestream_read_binary_file(CuTest *tc)
{
   const char *test_filename = "test_filestream_bin_tmp.bin";
   uint8_t bin_data[256];
   uint32_t i;
   for (i = 0; i < 256; i++)
   {
      bin_data[i] = (uint8_t) i;
   }

   FILE *fh = fopen(test_filename, "wb");
   CuAssertPtrNotNull(tc, fh);
   fwrite(bin_data, 1, sizeof(bin_data), fh);
   fclose(fh);

   test_stream_sink_t sink;
   sink.buf = adt_bytearray_new();
   CuAssertPtrNotNull(tc, sink.buf);
   sink.open_count = 0;
   sink.close_count = 0;

   cutil_ifstream_handler_t handler;
   memset(&handler, 0, sizeof(handler));
   handler.arg = &sink;
   handler.open = sink_open;
   handler.close = sink_close;
   handler.write = sink_write;

   cutil_ifstream_t stream;
   cutil_ifstream_create(&stream, &handler);

   int rc = cutil_ifstream_read_binary_file(&stream, test_filename);
   CuAssertIntEquals(tc, 0, rc);
   CuAssertIntEquals(tc, 1, sink.open_count);
   CuAssertIntEquals(tc, 1, sink.close_count);

   CuAssertUIntEquals(tc, 256, adt_bytearray_length(sink.buf));
   CuAssertIntEquals(tc, 0, memcmp(bin_data, adt_bytearray_data(sink.buf), 256));

   cutil_ifstream_destroy(&stream);
   adt_bytearray_delete(sink.buf);
   remove(test_filename);
}

static void test_filestream_util_read_text_file(CuTest *tc)
{
   const char *test_filename = "test_filestream_util_tmp.txt";
   const char *content = "Alpha\nBeta\nGamma\n";
   FILE *fh = fopen(test_filename, "wb");
   CuAssertPtrNotNull(tc, fh);
   fputs(content, fh);
   fclose(fh);

   adt_bytearray_t *ba = cutil_ifstream_util_read_text_file(test_filename);
   CuAssertPtrNotNull(tc, ba);
   CuAssertUIntEquals(tc, (uint32_t) strlen(content), adt_bytearray_length(ba));
   CuAssertIntEquals(tc, 0, memcmp(content, adt_bytearray_data(ba), strlen(content)));

   adt_bytearray_delete(ba);
   remove(test_filename);

   // Non-existent file should return NULL
   adt_bytearray_t *null_ba = cutil_ifstream_util_read_text_file("nonexistent_file_12345.xyz");
   CuAssertPtrEquals(tc, NULL, null_ba);
}
