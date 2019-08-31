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
 * Purpose: This module implements basic services to traverse a diretory
 *          tree as specified by IEEE Std 1003.1.
 *
 * Author:  Daniel Gachet
 * Date:    05.05.2019
 */
#include "bbb/dirent.h"
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include "bbb/path.h"

extern dir_t* _opendir(const char* name);
extern struct dirent* _readdir(dir_t* dirp);
extern int _closedir(dir_t* dirp);
extern int _mkdir(const char* path, mode_t mode);
extern int _rmdir(const char* path);

dir_t* opendir(const char* name)
{
    size_t buf_sz = strlen(path_get_current_dir()) + 1 + strlen(name) + 1;
    char buf[buf_sz];
    const char* path = path_build_full_r(buf, buf_sz, name);
    return _opendir(path);
}

struct dirent* readdir(dir_t* dirp) { return _readdir(dirp); }

int closedir(dir_t* dirp) { return _closedir(dirp); }

int mkdir(const char* path, mode_t mode) { return _mkdir(path, mode); }

int rmdir(const char* path) { return _rmdir(path); }
