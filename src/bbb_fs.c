/**
 * Copyright 2019 University of Applied Sciences Western Switzerland / Fribourg
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * Project:	HEIA-FR / Embedded Systems Laboratory
 *
 * Abstract: Basic Virtual File System Services
 *
 * Purpose:	This module implements basic services for a virtual file system
 *
 * Author: 	Daniel Gachet
 * Date: 	08.05.2019
 */

/* GNU ARM Toolchain & newlib: system calls */
#include <stdarg.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <time.h>

#include "dirent.h"
#include "bbb_fs.h"
#include "path.h"
#include "vfs.h"

#include "console.h"

#if 1
#include <am335x_console.h>
#define putstr(x) am335x_console_puts(x)
#else
#define putstr(x) printf(x)
#endif

#define pr_debug(x...)
//#define pr_debug(x...) ({char buf[500]; sprintf(buf, x); putstr(buf);})

#define ARRAY_SIZE(x) ((int)(sizeof(x) / sizeof(x[0])))

// -----------------------------------------------------------------------------

struct vfs_file f[1024];

int _open(const char* name, int flags, int mode)
{
    size_t sz = strlen(path_get_current_dir()) + 1 + strlen(name) + 1;
    char buf[sz];
    name = path_build_full_r(buf, sz, name);

    pr_debug("flags:%08x, mode=%08x, name=%s\n", flags, mode, name);

    unsigned i = 0;
    while ((i < ARRAY_SIZE(f)) && (f[i].fops != 0)) i++;
    if (i >= ARRAY_SIZE(f)) return -1;

    f[i] = vfs_get_file(name);
    if (f[i].fops == 0) return -1;

    f[i].id = f[i].fops->open(&f[i], flags, mode);
    if (f[i].id == -1) return -1;

    return i;
}

int _read(int fd, char* ptr, int len)
{
    return f[fd].fops->read(&f[fd], ptr, len);
}

int _write(int fd, char* ptr, int len)
{
    return f[fd].fops->write(&f[fd], ptr, len);
}

int _lseek(int fd, int offset, int whence)
{
    return f[fd].fops->lseek(&f[fd], offset, whence);
}

int _isatty(int fd) { return f[fd].fops->isatty(&f[fd]); }

int _close(int fd)
{
    int res = f[fd].fops->close(&f[fd]);
    f[fd].fops = 0;
    return res;
}

int fcntl(int fd, int cmd, ...)
{
    va_list args;
    va_start(args, cmd);
    int res = f[fd].fops->fcntl(&f[fd], cmd, args);
    va_end(args);
    return res;
}

int _fstat(int fd, struct stat* st)
{
    return f[fd].fops->fstat(&f[fd], st);
}

int _stat(const char* file, struct stat* st)
{
    file = path_build_full(file);

    struct vfs_dir dir = vfs_get_dir(file);
    if (dir.dops == 0) return -1;

    return dir.dops->stat(&dir, dir.name, st);
}

// --- directory services ------------------------------------------------------

dir_t* _opendir(const char* name)
{
    pr_debug("_opendir: %s\n", name);

    name = path_build_full(name);

    struct vfs_dir dir = vfs_get_dir(name);
    if (dir.dops == 0) return 0;

    dir.dirp = dir.dops->opendir(&dir, dir.name);
    if (dir.dirp == 0) return 0;

    struct vfs_dir* d = calloc(1, sizeof(*d));
    if (d == 0) return 0;

    *d = dir;

    return (dir_t*)d;
}

struct dirent* _readdir(dir_t* dirp)
{
    struct vfs_dir* dir = (struct vfs_dir*)dirp;
    return dir->dops->readdir(dir, dir->dirp);
}

int _closedir(dir_t* dirp)
{
    struct vfs_dir* dir = (struct vfs_dir*)dirp;
    dir->dops->closedir(dir, dir->dirp);
    free(dir);
    return 0;
}

int _mkdir(const char* path, mode_t mode)
{
    struct vfs_dir dir = vfs_get_dir(path);
    if (dir.dops == 0) return -1;
    pr_debug("mkdir: path=%s, name=%s\n", path, dir.name);

    return dir.dops->mkdir(&dir, dir.name, mode);
}

int _rmdir(const char* path)
{
    struct vfs_dir dir = vfs_get_dir(path);
    if (dir.dops == 0) return -1;
    pr_debug("rmdir: path=%s, name=%s\n", path, dir.name);

    return dir.dops->rmdir(&dir, dir.name);
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

int bbb_fs_init()
{
    vfs_init();
    console_init();
    f[0].fops = console_get_fops();
    f[1].fops = console_get_fops();
    f[2].fops = console_get_fops();
    return 0;
}
