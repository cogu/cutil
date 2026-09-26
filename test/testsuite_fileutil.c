/*****************************************************************************
* \file      testsuite_fileutil.c
* \author    Conny Gustafsson
* \date      2026-09-07
* \brief     Unit tests for fileutil
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
#include <stdlib.h>
#ifdef _WIN32
#include <direct.h>
#define MKDIR(d) _mkdir(d)
#define RMDIR(d) _rmdir(d)
#else
#include <sys/stat.h>
#include <unistd.h>
#define MKDIR(d) mkdir(d, 0777)
#define RMDIR(d) rmdir(d)
#endif
#include "CuTest.h"
#include "fileutil.h"
#include "adt_str.h"
#ifdef MEM_LEAK_CHECK
#include "CMemLeak.h"
#endif

//////////////////////////////////////////////////////////////////////////////
// PRIVATE FUNCTION PROTOTYPES
//////////////////////////////////////////////////////////////////////////////
static void test_is_dir_null_and_empty(CuTest *tc);
static void test_is_dir_nonexistent(CuTest *tc);
static void test_is_dir_regular_file(CuTest *tc);
static void test_is_dir_existing_directory(CuTest *tc);
static void test_is_dir_trailing_slashes(CuTest *tc);
static void test_is_dir_root(CuTest *tc);

static void test_path_join_basic(CuTest *tc);
static void test_path_join_dir_with_trailing_slash(CuTest *tc);
static void test_path_join_filename_with_leading_slash(CuTest *tc);
static void test_path_join_both_slashes(CuTest *tc);
static void test_path_join_multiple_slashes(CuTest *tc);
static void test_path_join_null_and_empty_dir(CuTest *tc);
static void test_path_join_null_and_empty_filename(CuTest *tc);
static void test_path_join_both_null_or_empty(CuTest *tc);
static void test_path_join_backslash_support(CuTest *tc);

static void test_file_exists_basic(CuTest *tc);
static void test_path_append_extension(CuTest *tc);
static void test_path_replace_extension(CuTest *tc);
static void test_read_write_binary_file(CuTest *tc);

//////////////////////////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
//////////////////////////////////////////////////////////////////////////////
CuSuite* testsuite_fileutil(void)
{
   CuSuite* suite = CuSuiteNew();
   SUITE_ADD_TEST(suite, test_is_dir_null_and_empty);
   SUITE_ADD_TEST(suite, test_is_dir_nonexistent);
   SUITE_ADD_TEST(suite, test_is_dir_regular_file);
   SUITE_ADD_TEST(suite, test_is_dir_existing_directory);
   SUITE_ADD_TEST(suite, test_is_dir_trailing_slashes);
   SUITE_ADD_TEST(suite, test_is_dir_root);

   SUITE_ADD_TEST(suite, test_path_join_basic);
   SUITE_ADD_TEST(suite, test_path_join_dir_with_trailing_slash);
   SUITE_ADD_TEST(suite, test_path_join_filename_with_leading_slash);
   SUITE_ADD_TEST(suite, test_path_join_both_slashes);
   SUITE_ADD_TEST(suite, test_path_join_multiple_slashes);
   SUITE_ADD_TEST(suite, test_path_join_null_and_empty_dir);
   SUITE_ADD_TEST(suite, test_path_join_null_and_empty_filename);
   SUITE_ADD_TEST(suite, test_path_join_both_null_or_empty);
   SUITE_ADD_TEST(suite, test_path_join_backslash_support);

   SUITE_ADD_TEST(suite, test_file_exists_basic);
   SUITE_ADD_TEST(suite, test_path_append_extension);
   SUITE_ADD_TEST(suite, test_path_replace_extension);
   SUITE_ADD_TEST(suite, test_read_write_binary_file);

   return suite;
}

//////////////////////////////////////////////////////////////////////////////
// PRIVATE FUNCTIONS
//////////////////////////////////////////////////////////////////////////////

static void test_is_dir_null_and_empty(CuTest *tc)
{
   CuAssertTrue(tc, !cutil_is_dir(NULL));
   CuAssertTrue(tc, !cutil_is_dir(""));
}

static void test_is_dir_nonexistent(CuTest *tc)
{
   CuAssertTrue(tc, !cutil_is_dir("this_directory_should_not_exist_xyz123"));
}

static void test_is_dir_regular_file(CuTest *tc)
{
   const char *filepath = "test_regular_file.tmp";
   FILE *fh = fopen(filepath, "w");
   CuAssertPtrNotNull(tc, fh);
   fputs("hello", fh);
   fclose(fh);

   CuAssertTrue(tc, !cutil_is_dir(filepath));
   remove(filepath);
}

static void test_is_dir_existing_directory(CuTest *tc)
{
   const char *dirname = "test_temp_dir";
   CuAssertIntEquals(tc, 0, MKDIR(dirname));

   CuAssertTrue(tc, cutil_is_dir(dirname));
   CuAssertTrue(tc, cutil_is_dir("."));

   CuAssertIntEquals(tc, 0, RMDIR(dirname));
}

static void test_is_dir_trailing_slashes(CuTest *tc)
{
   const char *dirname = "test_trailing_slash_dir";
   CuAssertIntEquals(tc, 0, MKDIR(dirname));

   CuAssertTrue(tc, cutil_is_dir("test_trailing_slash_dir/"));
   CuAssertTrue(tc, cutil_is_dir("test_trailing_slash_dir//"));
   CuAssertTrue(tc, cutil_is_dir("test_trailing_slash_dir///"));

   CuAssertIntEquals(tc, 0, RMDIR(dirname));
}

static void test_is_dir_root(CuTest *tc)
{
#ifndef _WIN32
   CuAssertTrue(tc, cutil_is_dir("/"));
#else
   (void)tc;
#endif
}

static void test_path_join_basic(CuTest *tc)
{
   adt_str_t *path = cutil_path_join("dir", "file");
   CuAssertPtrNotNull(tc, path);
   CuAssertStrEquals(tc, "dir/file", adt_str_cstr(path));
   adt_str_delete(path);
}

static void test_path_join_dir_with_trailing_slash(CuTest *tc)
{
   adt_str_t *path = cutil_path_join("dir/", "file");
   CuAssertPtrNotNull(tc, path);
   CuAssertStrEquals(tc, "dir/file", adt_str_cstr(path));
   adt_str_delete(path);
}

static void test_path_join_filename_with_leading_slash(CuTest *tc)
{
   adt_str_t *path = cutil_path_join("dir", "/file");
   CuAssertPtrNotNull(tc, path);
   CuAssertStrEquals(tc, "dir/file", adt_str_cstr(path));
   adt_str_delete(path);
}

static void test_path_join_both_slashes(CuTest *tc)
{
   adt_str_t *path = cutil_path_join("dir/", "/file");
   CuAssertPtrNotNull(tc, path);
   CuAssertStrEquals(tc, "dir/file", adt_str_cstr(path));
   adt_str_delete(path);
}

static void test_path_join_multiple_slashes(CuTest *tc)
{
   adt_str_t *path = cutil_path_join("dir/", "///file");
   CuAssertPtrNotNull(tc, path);
   CuAssertStrEquals(tc, "dir/file", adt_str_cstr(path));
   adt_str_delete(path);
}

static void test_path_join_null_and_empty_dir(CuTest *tc)
{
   adt_str_t *path1 = cutil_path_join(NULL, "file");
   CuAssertPtrNotNull(tc, path1);
   CuAssertStrEquals(tc, "file", adt_str_cstr(path1));
   adt_str_delete(path1);

   adt_str_t *path2 = cutil_path_join("", "file");
   CuAssertPtrNotNull(tc, path2);
   CuAssertStrEquals(tc, "file", adt_str_cstr(path2));
   adt_str_delete(path2);
}

static void test_path_join_null_and_empty_filename(CuTest *tc)
{
   adt_str_t *path1 = cutil_path_join("dir", NULL);
   CuAssertPtrNotNull(tc, path1);
   CuAssertStrEquals(tc, "dir", adt_str_cstr(path1));
   adt_str_delete(path1);

   adt_str_t *path2 = cutil_path_join("dir", "");
   CuAssertPtrNotNull(tc, path2);
   CuAssertStrEquals(tc, "dir", adt_str_cstr(path2));
   adt_str_delete(path2);
}

static void test_path_join_both_null_or_empty(CuTest *tc)
{
   adt_str_t *path1 = cutil_path_join(NULL, NULL);
   CuAssertPtrNotNull(tc, path1);
   CuAssertStrEquals(tc, "", adt_str_cstr(path1));
   adt_str_delete(path1);

   adt_str_t *path2 = cutil_path_join("", "");
   CuAssertPtrNotNull(tc, path2);
   CuAssertStrEquals(tc, "", adt_str_cstr(path2));
   adt_str_delete(path2);
}

static void test_path_join_backslash_support(CuTest *tc)
{
   adt_str_t *path1 = cutil_path_join("dir\\", "file");
   CuAssertPtrNotNull(tc, path1);
   CuAssertStrEquals(tc, "dir\\file", adt_str_cstr(path1));
   adt_str_delete(path1);

   adt_str_t *path2 = cutil_path_join("dir\\", "\\file");
   CuAssertPtrNotNull(tc, path2);
   CuAssertStrEquals(tc, "dir\\file", adt_str_cstr(path2));
   adt_str_delete(path2);
}

static void test_file_exists_basic(CuTest *tc)
{
   CuAssertTrue(tc, !cutil_file_exists(NULL));
   CuAssertTrue(tc, !cutil_file_exists(""));
   CuAssertTrue(tc, !cutil_file_exists("non_existent_file_xyz_12345.tmp"));

   // Test with directory - should return false
   const char *test_dir = "test_exists_dir";
   MKDIR(test_dir);
   CuAssertTrue(tc, !cutil_file_exists(test_dir));
   RMDIR(test_dir);

   // Create a temporary file and verify
   const char *test_file = "test_exists_file.tmp";
   FILE *fh = fopen(test_file, "wb");
   CuAssertPtrNotNull(tc, fh);
   fputc('A', fh);
   fclose(fh);

   CuAssertTrue(tc, cutil_file_exists(test_file));
   remove(test_file);
   CuAssertTrue(tc, !cutil_file_exists(test_file));
}

static void test_path_append_extension(CuTest *tc)
{
   adt_str_t *s1 = cutil_path_append_extension(NULL, ".sig");
   CuAssertPtrEquals(tc, NULL, s1);

   adt_str_t *s2 = cutil_path_append_extension("node.apx", ".sig");
   CuAssertPtrNotNull(tc, s2);
   CuAssertStrEquals(tc, "node.apx.sig", adt_str_cstr(s2));
   adt_str_delete(s2);

   // Extension without leading dot
   adt_str_t *s3 = cutil_path_append_extension("node.apx", "sig");
   CuAssertPtrNotNull(tc, s3);
   CuAssertStrEquals(tc, "node.apx.sig", adt_str_cstr(s3));
   adt_str_delete(s3);

   // Path with directories
   adt_str_t *s4 = cutil_path_append_extension("/path/to/my_node.apx", ".sig");
   CuAssertPtrNotNull(tc, s4);
   CuAssertStrEquals(tc, "/path/to/my_node.apx.sig", adt_str_cstr(s4));
   adt_str_delete(s4);
}

static void test_path_replace_extension(CuTest *tc)
{
   adt_str_t *s1 = cutil_path_replace_extension(NULL, ".sig");
   CuAssertPtrEquals(tc, NULL, s1);

   adt_str_t *s2 = cutil_path_replace_extension("node.apx", ".sig");
   CuAssertPtrNotNull(tc, s2);
   CuAssertStrEquals(tc, "node.sig", adt_str_cstr(s2));
   adt_str_delete(s2);

   // Path with directory separators and dots in directory name
   adt_str_t *s3 = cutil_path_replace_extension("dir.v1/node.apx", ".sig");
   CuAssertPtrNotNull(tc, s3);
   CuAssertStrEquals(tc, "dir.v1/node.sig", adt_str_cstr(s3));
   adt_str_delete(s3);

   // File without extension
   adt_str_t *s4 = cutil_path_replace_extension("node", ".sig");
   CuAssertPtrNotNull(tc, s4);
   CuAssertStrEquals(tc, "node.sig", adt_str_cstr(s4));
   adt_str_delete(s4);
}

static void test_read_write_binary_file(CuTest *tc)
{
   const char *test_file = "test_bin_rw.tmp";
   uint8_t write_data[64];
   for (size_t i = 0; i < sizeof(write_data); i++)
   {
      write_data[i] = (uint8_t)(i ^ 0x5A);
   }

   int rc = cutil_write_binary_file(test_file, write_data, sizeof(write_data));
   CuAssertIntEquals(tc, 0, rc);

   uint8_t read_buf[128];
   memset(read_buf, 0, sizeof(read_buf));
   size_t bytes_read = 0;

   rc = cutil_read_binary_file(test_file, read_buf, sizeof(read_buf), &bytes_read);
   CuAssertIntEquals(tc, 0, rc);
   CuAssertIntEquals(tc, (int)sizeof(write_data), (int)bytes_read);
   CuAssertIntEquals(tc, 0, memcmp(write_data, read_buf, sizeof(write_data)));

   remove(test_file);

   // Reading non-existent file returns error
   rc = cutil_read_binary_file("non_existent_file_98765.tmp", read_buf, sizeof(read_buf), NULL);
   CuAssertIntEquals(tc, -1, rc);
}
