/**
 * Copyright 2018 University of Applied Sciences Western Switzerland / Fribourg
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
 * Date:    20.02.2019
 */

#include "vfs.h"

static struct vfs_dir vfs_get_dir_internal(const char* name)
{
    struct vfs_dir dir = {.dops = 0, .name = name};
    return dir;
}

static struct vfs_file vfs_get_file_internal(const char* name)
{
    struct vfs_file file = {
        .fops = 0,
        .name = name,
    };
    return file;
}

static vfs_get_dir_t get_dir   = vfs_get_dir_internal;
static vfs_get_file_t get_file = vfs_get_file_internal;

void vfs_init() {}

void vfs_set_method_get_dir(vfs_get_dir_t method) { get_dir = method; }

void vfs_set_method_get_file(vfs_get_file_t method) { get_file = method; }

struct vfs_dir vfs_get_dir(const char* name) { return get_dir(name); }

struct vfs_file vfs_get_file(const char* name) { return get_file(name); }
