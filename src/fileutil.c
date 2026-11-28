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
#ifndef _WIN32
# ifndef _POSIX_C_SOURCE
#  define _POSIX_C_SOURCE 200809L
# endif
# ifndef _DEFAULT_SOURCE
#  define _DEFAULT_SOURCE
# endif
#endif
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#ifdef _WIN32
# ifndef WIN32_LEAN_AND_MEAN
#  define WIN32_LEAN_AND_MEAN
# endif
# include <Windows.h>
# include <direct.h>
# include <io.h>
# include <process.h>
# include <sys/utime.h>
#else
# include <dirent.h>
# include <fcntl.h>
# include <sys/file.h>
# include <time.h>
# include <unistd.h>
#endif
#include "fileutil.h"

//////////////////////////////////////////////////////////////////////////////
// PRIVATE CONSTANTS AND DATA TYPES
//////////////////////////////////////////////////////////////////////////////
#ifndef CUTIL_PATH_MAX
# define CUTIL_PATH_MAX 4096
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
    adt_str_t *result = adt_str_new_bstr((const uint8_t *)filepath, (const uint8_t *)filepath + base_len);
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

static int make_single_dir(const char *path)
{
#ifdef _WIN32
  if (_mkdir(path) == 0)
  {
    return 0;
  }
#else
  if (mkdir(path, 0755) == 0)
  {
    return 0;
  }
#endif
  if (errno == EEXIST)
  {
    if (cutil_is_dir(path))
    {
      return 0;
    }
    errno = ENOTDIR;
  }
  return -1;
}

int cutil_mkdir_p(const char *path)
{
  if (path == NULL)
  {
    errno = EINVAL;
    return -1;
  }
  size_t len = strlen(path);
  if (len == 0 || len >= CUTIL_PATH_MAX)
  {
    errno = (len == 0) ? EINVAL : ENAMETOOLONG;
    return -1;
  }
  if (cutil_is_dir(path))
  {
    return 0;
  }

   // Strip trailing slashes (preserve root "/" or "C:\")
  while (len > 1 && (path[len - 1] == '/' || path[len - 1] == '\\'))
  {
#ifdef _WIN32
    if (len == 3 && path[1] == ':')
    {
      break;
    }
#endif
    len--;
  }

  char buf[CUTIL_PATH_MAX];
  memcpy(buf, path, len);
  buf[len] = '\0';

  size_t start_idx = 1;
#ifdef _WIN32
  if (len >= 2 && buf[1] == ':')
  {
    start_idx = (len >= 3 && (buf[2] == '/' || buf[2] == '\\')) ? 3 : 2;
  }
#endif

  for (size_t i = start_idx; i < len; i++)
  {
    if (buf[i] == '/' || buf[i] == '\\')
    {
      char saved = buf[i];
      buf[i] = '\0';
      if (buf[0] != '\0' && make_single_dir(buf) != 0)
      {
        return -1;
      }
      buf[i] = saved;
    }
  }
  return make_single_dir(buf);
}

int cutil_dir_for_each(const char *dir_path, cutil_dir_entry_cb cb, void *arg)
{
  if (dir_path == NULL || cb == NULL || dir_path[0] == '\0')
  {
    errno = EINVAL;
    return -1;
  }

#ifdef _WIN32
  size_t dir_len = strlen(dir_path);
  if (dir_len + 3 >= CUTIL_PATH_MAX)
  {
    errno = ENAMETOOLONG;
    return -1;
  }
  char pattern[CUTIL_PATH_MAX];
  memcpy(pattern, dir_path, dir_len);
  if (pattern[dir_len - 1] == '/' || pattern[dir_len - 1] == '\\')
  {
    pattern[dir_len] = '*';
    pattern[dir_len + 1] = '\0';
  }
  else
  {
    pattern[dir_len] = '\\';
    pattern[dir_len + 1] = '*';
    pattern[dir_len + 2] = '\0';
  }

  WIN32_FIND_DATAA fd;
  HANDLE hFind = FindFirstFileA(pattern, &fd);
  if (hFind == INVALID_HANDLE_VALUE)
  {
    DWORD err = GetLastError();
    if (err == ERROR_FILE_NOT_FOUND || err == ERROR_NO_MORE_FILES)
    {
      return 0;
    }
    errno = (err == ERROR_ACCESS_DENIED) ? EACCES : ENOENT;
    return -1;
  }

  do
  {
    if (strcmp(fd.cFileName, ".") == 0 || strcmp(fd.cFileName, "..") == 0)
    {
      continue;
    }
    cutil_dir_entry_t entry;
    memset(&entry, 0, sizeof(entry));
    entry.name = fd.cFileName;
    entry.is_symlink = ((fd.dwFileAttributes & FILE_ATTRIBUTE_REPARSE_POINT) != 0);
    entry.is_dir = (!entry.is_symlink) && ((fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0);
    entry.is_reg = (!entry.is_symlink) && (!entry.is_dir);
    if (entry.is_reg)
    {
      entry.size = (((uint64_t)fd.nFileSizeHigh) << 32) | (uint64_t)fd.nFileSizeLow;
    }
    uint64_t ft64 = (((uint64_t)fd.ftLastWriteTime.dwHighDateTime) << 32) | (uint64_t)fd.ftLastWriteTime.dwLowDateTime;
    uint64_t sec = ft64 / 10000000ULL;
    entry.mtime_sec = (sec >= 11644473600ULL) ? (int64_t)(sec - 11644473600ULL) : 0;

    if (!cb(arg, &entry))
    {
      break;
    }
  }
  while (FindNextFileA(hFind, &fd));

  FindClose(hFind);
  return 0;
#else
  DIR *dir = opendir(dir_path);
  if (dir == NULL)
  {
    return -1;
  }

  size_t dir_len = strlen(dir_path);
  bool has_sep = (dir_len > 0 && dir_path[dir_len - 1] == '/');
  struct dirent *de = NULL;
  while ((de = readdir(dir)) != NULL)
  {
    if (strcmp(de->d_name, ".") == 0 || strcmp(de->d_name, "..") == 0)
    {
      continue;
    }
    size_t name_len = strlen(de->d_name);
    if (dir_len + (has_sep ? 0u : 1u) + name_len + 1u >= CUTIL_PATH_MAX)
    {
      continue;
    }
    char full_path[CUTIL_PATH_MAX];
    memcpy(full_path, dir_path, dir_len);
    size_t pos = dir_len;
    if (!has_sep)
    {
      full_path[pos++] = '/';
    }
    memcpy(full_path + pos, de->d_name, name_len + 1u);

    struct stat st;
    if (lstat(full_path, &st) != 0)
    {
      continue;
    }
    cutil_dir_entry_t entry;
    memset(&entry, 0, sizeof(entry));
    entry.name = de->d_name;
    entry.is_symlink = S_ISLNK(st.st_mode);
    entry.is_dir = S_ISDIR(st.st_mode);
    entry.is_reg = S_ISREG(st.st_mode);
    entry.size = (entry.is_reg && st.st_size > 0) ? (uint64_t)st.st_size : 0u;
    entry.mtime_sec = (int64_t)st.st_mtime;

    if (!cb(arg, &entry))
    {
      break;
    }
  }
  closedir(dir);
  return 0;
#endif
}

int cutil_read_regular_file(
  const char *path, uint8_t *buf, size_t expected_size, size_t *out_actual_size, int64_t *out_mtime_sec)
{
  if (path == NULL || path[0] == '\0' || (buf == NULL && expected_size > 0))
  {
    errno = EINVAL;
    return -1;
  }

#ifdef _WIN32
  DWORD attrs = GetFileAttributesA(path);
  if (attrs == INVALID_FILE_ATTRIBUTES)
  {
    DWORD err = GetLastError();
    errno = (err == ERROR_ACCESS_DENIED) ? EACCES : ENOENT;
    return -1;
  }
  if ((attrs & FILE_ATTRIBUTE_REPARSE_POINT) != 0 || (attrs & FILE_ATTRIBUTE_DIRECTORY) != 0)
  {
    errno = EINVAL;
    return -1;
  }
  FILE *fh = fopen(path, "rb");
  if (fh == NULL)
  {
    return -1;
  }
  struct __stat64 st;
  if (_fstat64(_fileno(fh), &st) != 0)
  {
    int saved_errno = errno;
    fclose(fh);
    errno = saved_errno;
    return -1;
  }
  if (!S_ISREG(st.st_mode))
  {
    fclose(fh);
    errno = EINVAL;
    return -1;
  }
  if (out_actual_size != NULL)
  {
    *out_actual_size = (st.st_size > 0) ? (size_t)st.st_size : 0u;
  }
  if (out_mtime_sec != NULL)
  {
    *out_mtime_sec = (int64_t)st.st_mtime;
  }
  if (st.st_size < 0 || (uint64_t)st.st_size != (uint64_t)expected_size)
  {
    fclose(fh);
    return -2;
  }
  if (expected_size > 0)
  {
    size_t n = fread(buf, 1, expected_size, fh);
    if (n != expected_size)
    {
      int is_err = ferror(fh);
      int saved_errno = errno;
      fclose(fh);
      errno = saved_errno;
      return is_err ? -1 : -2;
    }
  }
  fclose(fh);
  return 0;
#else
  int flags = O_RDONLY;
# ifdef O_NOFOLLOW
  flags |= O_NOFOLLOW;
# endif
  int fd = open(path, flags);
  if (fd < 0)
  {
    return -1;
  }
  struct stat st;
  if (fstat(fd, &st) != 0)
  {
    int saved_errno = errno;
    close(fd);
    errno = saved_errno;
    return -1;
  }
  if (!S_ISREG(st.st_mode))
  {
    close(fd);
    errno = EINVAL;
    return -1;
  }
  if (out_actual_size != NULL)
  {
    *out_actual_size = (st.st_size > 0) ? (size_t)st.st_size : 0u;
  }
  if (out_mtime_sec != NULL)
  {
    *out_mtime_sec = (int64_t)st.st_mtime;
  }
  if (st.st_size < 0 || (uint64_t)st.st_size != (uint64_t)expected_size)
  {
    close(fd);
    return -2;
  }
  size_t total = 0u;
  while (total < expected_size)
  {
    ssize_t n = read(fd, buf + total, expected_size - total);
    if (n < 0)
    {
      if (errno == EINTR)
      {
        continue;
      }
      int saved_errno = errno;
      close(fd);
      errno = saved_errno;
      return -1;
    }
    if (n == 0)
    {
      close(fd);
      return -2;
    }
    total += (size_t)n;
  }
  close(fd);
  return 0;
#endif
}

int cutil_file_replace_atomic(const char *src_path, const char *dst_path)
{
  if (src_path == NULL || dst_path == NULL || src_path[0] == '\0' || dst_path[0] == '\0')
  {
    errno = EINVAL;
    return -1;
  }
#ifdef _WIN32
  if (!MoveFileExA(src_path, dst_path, MOVEFILE_REPLACE_EXISTING))
  {
    DWORD err = GetLastError();
    errno = (err == ERROR_ACCESS_DENIED) ? EACCES : EIO;
    return -1;
  }
  return 0;
#else
  return rename(src_path, dst_path);
#endif
}

int cutil_file_touch_mtime(const char *path)
{
  if (path == NULL || path[0] == '\0')
  {
    errno = EINVAL;
    return -1;
  }
#ifdef _WIN32
  return _utime(path, NULL);
#else
  return utimensat(AT_FDCWD, path, NULL, AT_SYMLINK_NOFOLLOW);
#endif
}

int cutil_file_set_mtime(const char *path, int64_t mtime_sec)
{
  if (path == NULL || path[0] == '\0')
  {
    errno = EINVAL;
    return -1;
  }
#ifdef _WIN32
  struct _utimbuf times;
  times.actime = (time_t)mtime_sec;
  times.modtime = (time_t)mtime_sec;
  return _utime(path, &times);
#else
  struct timespec ts[2];
  ts[0].tv_sec = (time_t)mtime_sec;
  ts[0].tv_nsec = 0;
  ts[1].tv_sec = (time_t)mtime_sec;
  ts[1].tv_nsec = 0;
  return utimensat(AT_FDCWD, path, ts, AT_SYMLINK_NOFOLLOW);
#endif
}

int cutil_fsync_file(FILE *fh)
{
  if (fh == NULL)
  {
    errno = EINVAL;
    return -1;
  }
  if (fflush(fh) != 0)
  {
    return -1;
  }
#ifdef _WIN32
  int fd = _fileno(fh);
  if (fd < 0)
  {
    return -1;
  }
  return _commit(fd);
#else
  int fd = fileno(fh);
  if (fd < 0)
  {
    return -1;
  }
  return fsync(fd);
#endif
}

void cutil_file_lock_init(cutil_file_lock_t *lock)
{
  if (lock != NULL)
  {
#ifdef _WIN32
    lock->handle = NULL;
#else
    lock->fd = -1;
#endif
    lock->locked = false;
  }
}

int cutil_file_lock_try_acquire(cutil_file_lock_t *lock, const char *path)
{
  if (lock == NULL || path == NULL || path[0] == '\0')
  {
    errno = EINVAL;
    return -1;
  }
  cutil_file_lock_release(lock);

#ifdef _WIN32
  HANDLE h = CreateFileA(path, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_ALWAYS,
    FILE_ATTRIBUTE_NORMAL, NULL);
  if (h == INVALID_HANDLE_VALUE)
  {
    DWORD err = GetLastError();
    errno = (err == ERROR_ACCESS_DENIED) ? EACCES : EIO;
    return -1;
  }

  OVERLAPPED ov;
  memset(&ov, 0, sizeof(ov));
   /* Lock a 1-byte sentinel beyond EOF so the PID text at offset 0 remains readable on Windows. */
  ov.Offset = 0x00010000u;
  ov.OffsetHigh = 0u;
  if (!LockFileEx(h, LOCKFILE_EXCLUSIVE_LOCK | LOCKFILE_FAIL_IMMEDIATELY, 0, 1, 0, &ov))
  {
    DWORD err = GetLastError();
    CloseHandle(h);
    if (err == ERROR_LOCK_VIOLATION)
    {
      errno = EWOULDBLOCK;
      return 1;
    }
    errno = (err == ERROR_ACCESS_DENIED) ? EACCES : EIO;
    return -1;
  }

  char pid_buf[32];
  int pid_len = snprintf(pid_buf, sizeof(pid_buf), "%lu\n", (unsigned long)_getpid());
  if (pid_len > 0)
  {
    SetFilePointer(h, 0, NULL, FILE_BEGIN);
    SetEndOfFile(h);
    DWORD written = 0;
    (void)WriteFile(h, pid_buf, (DWORD)pid_len, &written, NULL);
  }

  lock->handle = (void *)h;
  lock->locked = true;
  return 0;
#else
  int flags = O_CREAT | O_RDWR;
# ifdef O_CLOEXEC
  flags |= O_CLOEXEC;
# endif
# ifdef O_NOFOLLOW
  flags |= O_NOFOLLOW;
# endif
  int fd = open(path, flags, 0644);
  if (fd < 0)
  {
    return -1;
  }
  if (flock(fd, LOCK_EX | LOCK_NB) != 0)
  {
    int saved_errno = errno;
    close(fd);
    errno = saved_errno;
    if (saved_errno == EWOULDBLOCK || saved_errno == EAGAIN)
    {
      return 1;
    }
    return -1;
  }

  char pid_buf[32];
  int pid_len = snprintf(pid_buf, sizeof(pid_buf), "%lu\n", (unsigned long)getpid());
  if (pid_len > 0 && ftruncate(fd, 0) == 0)
  {
    (void)lseek(fd, 0, SEEK_SET);
    ssize_t written = write(fd, pid_buf, (size_t)pid_len);
    (void)written;
  }

  lock->fd = fd;
  lock->locked = true;
  return 0;
#endif
}

void cutil_file_lock_release(cutil_file_lock_t *lock)
{
  if (lock != NULL)
  {
#ifdef _WIN32
    if (lock->handle != NULL)
    {
      if (lock->locked)
      {
        OVERLAPPED ov;
        memset(&ov, 0, sizeof(ov));
        ov.Offset = 0x00010000u;
        ov.OffsetHigh = 0u;
        UnlockFileEx((HANDLE)lock->handle, 0, 1, 0, &ov);
      }
      CloseHandle((HANDLE)lock->handle);
      lock->handle = NULL;
    }
#else
    if (lock->fd >= 0)
    {
      if (lock->locked)
      {
        (void)flock(lock->fd, LOCK_UN);
      }
      close(lock->fd);
      lock->fd = -1;
    }
#endif
    lock->locked = false;
  }
}

bool cutil_file_lock_is_held(const cutil_file_lock_t *lock) { return (lock != NULL) && lock->locked; }
