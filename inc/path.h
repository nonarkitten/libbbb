#pragma once
#ifndef _PATH_H
#define _PATH_H
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
 * Abstract:Basic Virtual File System Services
 *
 * Purpose: This module provides services to handle pathes for directories
 *          of the Mini-Os virtual file system.
 *
 * Author:  Daniel Gachet
 * Date:    08.05.2019
 */

#include <stddef.h>

/**
 * method to set current working directory.
 *
 * if path is an absolute path, the new current directory will be set to path
 * otherwise the new current currently will be built by joining the actual
 * current directory with the specified path
 *
 * @param path new current path
 * @return new current path
 */
extern const char* path_set_current_dir(const char* path);

/**
 * method to purify/normalize the path string
 *
 * @param path string to purify
 * @return purified path string
 */
extern const char* path_purify(char* path);

/**
 * method to get current working directory.
 *
 * @return current path
 */
extern const char* path_get_current_dir();

/**
 * method to build full path from current working directory and specified path
 *
 * if path is an absolute path, fullpath will be set to path otherwise full path
 * will be built by joining the actual current directory with the specified path
 *
 * @param path path from which full path is requested
 * @return full path
 */
extern const char* path_build_full(const char* path);

/**
 * method to build full path from current working directory and specified path
 *
 * if path is an absolute path, fullpath will be set to path otherwise full path
 * will be built by joining the actual current directory with the specified path
 *
 * @param buf buffer to store full path
 * @param buf_sz size of the buffer in bytes
 * @param path path from which full path is requested
 * @return full path
 */
extern const char* path_build_full_r(char* buf,
                                     size_t buf_sz,
                                     const char* path);

#endif
