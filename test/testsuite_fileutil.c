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
