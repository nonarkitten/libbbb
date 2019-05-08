#pragma once
#ifndef dirent_h
#define dirent_h
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
 * Purpose:	This module implements basic services to traverse a diretory
 *			tree as specified by IEEE Std 1003.1.
 *
 * Author: 	Daniel Gachet
 * Date: 	08.05.2019
 */

#include <stdint.h>

typedef struct directory dir_t;

enum filetype {
    DT_UNKNOWN = 0,
    DT_FIFO    = 1,
    DT_CHR     = 2,
    DT_DIR     = 4,
    DT_BLK     = 6,
    DT_REG     = 8,
    DT_LNK     = 10,
    DT_SOCK    = 12,
};

struct dirent {
    uint32_t d_ino;       /* inode number */
    enum filetype d_type; /* type of file; not supported
                             by all file system types */
    char d_name[256];     /* filename */
};

extern dir_t* opendir(const char* name);
extern struct dirent* readdir(dir_t* dirp);
extern int closedir(dir_t* dirp);

#endif
