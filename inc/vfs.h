#pragma once
#ifndef _VFS_H
#define _VFS_H
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
 * Project: HEIA-FR / Embedded Systems Laboratory
 *
 * Abstract: Basic Virtual File System Services
 *
 * Purpose: This module implements basic services for a virtual file
 *          system
 *
 * Author:  Daniel Gachet
 * Date:    08.05.2019
 */

#include <fcntl.h>
#include <stdarg.h>
#include <stdbool.h>
#include <sys/stat.h>
#include <sys/types.h>
#include "dirent.h"

#define MAX_FILENAME 256

struct vfs_file {
    struct vfs_file_ops* fops;
    const char* name;
    int id;
    int flags;
    int mode;
    void* user_data;
};

struct vfs_file_ops {
    int (*open)(struct vfs_file* f, int flags, int mode);
    int (*read)(struct vfs_file* f, char* ptr, int len);
    int (*write)(struct vfs_file* f, const char* ptr, int len);
    int (*lseek)(struct vfs_file* f, int offset, int whence);
    int (*isatty)(struct vfs_file* f);
    int (*fstat)(struct vfs_file* f, struct stat* st);
    int (*close)(struct vfs_file* f);
    int (*fcntl)(struct vfs_file* f, int cmd, va_list args);
};

struct vfs_dir {
    struct vfs_dir_ops* dops;
    const char* name;
    dir_t* dirp;
    struct vfs_dnode* dnode;
};

struct vfs_dir_ops {
    dir_t* (*opendir)(struct vfs_dir* d, const char* name);
    int (*closedir)(struct vfs_dir* d, dir_t* dirp);
    struct dirent* (*readdir)(struct vfs_dir* d, dir_t* dirp);

    int (*mkdir)(struct vfs_dir* d, const char* path, mode_t mode);
    int (*rmdir)(struct vfs_dir* d, const char* path);
    int (*stat)(struct vfs_dir* d, const char* name, struct stat* st);
};

typedef struct vfs_dir (*vfs_get_dir_t)(const char* name);
typedef struct vfs_file (*vfs_get_file_t)(const char* name);

extern void vfs_init();
extern void vfs_set_method_get_dir(vfs_get_dir_t method);
extern void vfs_set_method_get_file(vfs_get_file_t method);

extern struct vfs_dir vfs_get_dir(const char* name);
extern struct vfs_file vfs_get_file(const char* name);

#endif
