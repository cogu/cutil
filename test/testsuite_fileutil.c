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
#include <stdlib.h>
#include <string.h>
#ifdef _WIN32
# include <direct.h>
# define MKDIR(d) _mkdir(d)
# define RMDIR(d) _rmdir(d)
#else
# include <sys/stat.h>
# include <unistd.h>
# define MKDIR(d) mkdir(d, 0777)
# define RMDIR(d) rmdir(d)
#endif
#include "CuTest.h"
#include "adt_str.h"
#include "fileutil.h"

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
static void test_mkdir_p_nested_and_existing(CuTest *tc);
static void test_dir_for_each_lists_entries_and_stops(CuTest *tc);
static void test_read_regular_file_exact_mismatch_and_symlink(CuTest *tc);
static void test_file_replace_atomic_and_fsync(CuTest *tc);
static void test_file_mtime_set_and_touch(CuTest *tc);
static void test_file_lock_exclusive_and_release(CuTest *tc);

//////////////////////////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
//////////////////////////////////////////////////////////////////////////////
CuSuite *testsuite_fileutil(void)
{
  CuSuite *suite = CuSuiteNew();
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
  SUITE_ADD_TEST(suite, test_mkdir_p_nested_and_existing);
  SUITE_ADD_TEST(suite, test_dir_for_each_lists_entries_and_stops);
  SUITE_ADD_TEST(suite, test_read_regular_file_exact_mismatch_and_symlink);
  SUITE_ADD_TEST(suite, test_file_replace_atomic_and_fsync);
  SUITE_ADD_TEST(suite, test_file_mtime_set_and_touch);
  SUITE_ADD_TEST(suite, test_file_lock_exclusive_and_release);

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

static void test_mkdir_p_nested_and_existing(CuTest *tc)
{
  CuAssertIntEquals(tc, -1, cutil_mkdir_p(NULL));
  CuAssertIntEquals(tc, -1, cutil_mkdir_p(""));

  const char *nested = "test_mkdir_p_root/a/b/c";
  CuAssertIntEquals(tc, 0, cutil_mkdir_p(nested));
  CuAssertTrue(tc, cutil_is_dir(nested));
   // Idempotent when already existing
  CuAssertIntEquals(tc, 0, cutil_mkdir_p(nested));

   // Fails if target path is an existing regular file
  const char *file_path = "test_mkdir_p_root/a/regular.txt";
  CuAssertIntEquals(tc, 0, cutil_write_binary_file(file_path, (const uint8_t *)"x", 1u));
  CuAssertIntEquals(tc, -1, cutil_mkdir_p(file_path));

  remove(file_path);
  RMDIR("test_mkdir_p_root/a/b/c");
  RMDIR("test_mkdir_p_root/a/b");
  RMDIR("test_mkdir_p_root/a");
  RMDIR("test_mkdir_p_root");
}

typedef struct dir_collect_ctx_tag
{
  int count;
  int reg_count;
  int dir_count;
  bool saw_alpha;
  bool saw_beta;
  bool saw_sub;
  int stop_after;
} dir_collect_ctx_t;

static bool on_dir_entry(void *arg, const cutil_dir_entry_t *entry)
{
  dir_collect_ctx_t *ctx = (dir_collect_ctx_t *)arg;
  ctx->count++;
  if (entry->is_reg)
  {
    ctx->reg_count++;
  }
  if (entry->is_dir)
  {
    ctx->dir_count++;
  }
  if (strcmp(entry->name, "alpha.txt") == 0)
  {
    ctx->saw_alpha = (entry->is_reg && entry->size == 5u);
  }
  else if (strcmp(entry->name, "beta.txt") == 0)
  {
    ctx->saw_beta = (entry->is_reg && entry->size == 3u);
  }
  else if (strcmp(entry->name, "sub") == 0)
  {
    ctx->saw_sub = entry->is_dir;
  }
  if (ctx->stop_after > 0 && ctx->count >= ctx->stop_after)
  {
    return false;
  }
  return true;
}

static void test_dir_for_each_lists_entries_and_stops(CuTest *tc)
{
  const char *root = "test_dir_for_each_root";
  CuAssertIntEquals(tc, 0, cutil_mkdir_p("test_dir_for_each_root/sub"));
  CuAssertIntEquals(tc, 0, cutil_write_binary_file("test_dir_for_each_root/alpha.txt", (const uint8_t *)"hello", 5u));
  CuAssertIntEquals(tc, 0, cutil_write_binary_file("test_dir_for_each_root/beta.txt", (const uint8_t *)"abc", 3u));

  dir_collect_ctx_t ctx;
  memset(&ctx, 0, sizeof(ctx));
  CuAssertIntEquals(tc, 0, cutil_dir_for_each(root, on_dir_entry, &ctx));
  CuAssertIntEquals(tc, 3, ctx.count);
  CuAssertIntEquals(tc, 2, ctx.reg_count);
  CuAssertIntEquals(tc, 1, ctx.dir_count);
  CuAssertTrue(tc, ctx.saw_alpha);
  CuAssertTrue(tc, ctx.saw_beta);
  CuAssertTrue(tc, ctx.saw_sub);

   // Early stop when callback returns false
  memset(&ctx, 0, sizeof(ctx));
  ctx.stop_after = 1;
  CuAssertIntEquals(tc, 0, cutil_dir_for_each(root, on_dir_entry, &ctx));
  CuAssertIntEquals(tc, 1, ctx.count);

   // Missing directory returns -1
  CuAssertIntEquals(tc, -1, cutil_dir_for_each("no_such_dir_xyz_999", on_dir_entry, &ctx));

  remove("test_dir_for_each_root/alpha.txt");
  remove("test_dir_for_each_root/beta.txt");
  RMDIR("test_dir_for_each_root/sub");
  RMDIR(root);
}

static void test_read_regular_file_exact_mismatch_and_symlink(CuTest *tc)
{
  const char *path = "test_read_reg.tmp";
  const uint8_t payload[] = "abcdef";
  CuAssertIntEquals(tc, 0, cutil_write_binary_file(path, payload, 6u));

  uint8_t buf[16];
  size_t actual = 0u;
  int64_t mtime = 0;
  CuAssertIntEquals(tc, 0, cutil_read_regular_file(path, buf, 6u, &actual, &mtime));
  CuAssertIntEquals(tc, 6, (int)actual);
  CuAssertTrue(tc, mtime > 0);
  CuAssertIntEquals(tc, 0, memcmp(buf, payload, 6u));

   // Size mismatch returns -2 and reports actual size
  actual = 0u;
  CuAssertIntEquals(tc, -2, cutil_read_regular_file(path, buf, 4u, &actual, NULL));
  CuAssertIntEquals(tc, 6, (int)actual);

#ifndef _WIN32
  const char *link_path = "test_read_reg_link.tmp";
  remove(link_path);
  if (symlink(path, link_path) == 0)
  {
    CuAssertIntEquals(tc, -1, cutil_read_regular_file(link_path, buf, 6u, NULL, NULL));
    remove(link_path);
  }
#endif

  remove(path);
}

static void test_file_replace_atomic_and_fsync(CuTest *tc)
{
  const char *tmp_path = "test_atomic_src.tmp";
  const char *dst_path = "test_atomic_dst.tmp";

  CuAssertIntEquals(tc, 0, cutil_write_binary_file(dst_path, (const uint8_t *)"old", 3u));

  FILE *fh = fopen(tmp_path, "wb");
  CuAssertPtrNotNull(tc, fh);
  CuAssertIntEquals(tc, 4, (int)fwrite("new!", 1u, 4u, fh));
  CuAssertIntEquals(tc, 0, cutil_fsync_file(fh));
  fclose(fh);

  CuAssertIntEquals(tc, 0, cutil_file_replace_atomic(tmp_path, dst_path));
  CuAssertTrue(tc, !cutil_file_exists(tmp_path));
  CuAssertTrue(tc, cutil_file_exists(dst_path));

  uint8_t buf[8];
  CuAssertIntEquals(tc, 0, cutil_read_regular_file(dst_path, buf, 4u, NULL, NULL));
  CuAssertIntEquals(tc, 0, memcmp(buf, "new!", 4u));

  remove(dst_path);
}

static void test_file_mtime_set_and_touch(CuTest *tc)
{
  const char *path = "test_mtime.tmp";
  CuAssertIntEquals(tc, 0, cutil_write_binary_file(path, (const uint8_t *)"123", 3u));

  int64_t old_ts = 1600000000LL;
  CuAssertIntEquals(tc, 0, cutil_file_set_mtime(path, old_ts));

  uint8_t buf[4];
  int64_t mtime = 0;
  CuAssertIntEquals(tc, 0, cutil_read_regular_file(path, buf, 3u, NULL, &mtime));
  CuAssertTrue(tc, mtime == old_ts);

  CuAssertIntEquals(tc, 0, cutil_file_touch_mtime(path));
  mtime = 0;
  CuAssertIntEquals(tc, 0, cutil_read_regular_file(path, buf, 3u, NULL, &mtime));
  CuAssertTrue(tc, mtime > old_ts);

  remove(path);
}

static void test_file_lock_exclusive_and_release(CuTest *tc)
{
  const char *lock_path = "test_file_lock.lock";
  remove(lock_path);

  cutil_file_lock_t lock1;
  cutil_file_lock_t lock2;
  cutil_file_lock_init(&lock1);
  cutil_file_lock_init(&lock2);

  CuAssertTrue(tc, !cutil_file_lock_is_held(&lock1));
  CuAssertIntEquals(tc, 0, cutil_file_lock_try_acquire(&lock1, lock_path));
  CuAssertTrue(tc, cutil_file_lock_is_held(&lock1));

   // Second acquire on the same lock path must return 1 (already locked)
  CuAssertIntEquals(tc, 1, cutil_file_lock_try_acquire(&lock2, lock_path));
  CuAssertTrue(tc, !cutil_file_lock_is_held(&lock2));

   // Lock file should contain our numeric PID
  uint8_t pid_buf[32];
  size_t bytes_read = 0u;
  CuAssertIntEquals(tc, 0, cutil_read_binary_file(lock_path, pid_buf, sizeof(pid_buf) - 1u, &bytes_read));
  CuAssertTrue(tc, bytes_read > 0u);
  pid_buf[bytes_read] = '\0';
  CuAssertTrue(tc, strtol((const char *)pid_buf, NULL, 10) > 0);

   // After release, lock2 can acquire cleanly
  cutil_file_lock_release(&lock1);
  CuAssertTrue(tc, !cutil_file_lock_is_held(&lock1));
  CuAssertIntEquals(tc, 0, cutil_file_lock_try_acquire(&lock2, lock_path));
  CuAssertTrue(tc, cutil_file_lock_is_held(&lock2));

  cutil_file_lock_release(&lock2);
  remove(lock_path);
}
