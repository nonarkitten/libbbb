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
 * Purpose: This module provides services to handle pathes for directories
 *          of the Mini-Os virtual file system.
 *
 * Author:  Daniel Gachet
 * Date:    08.05.2019
 */

#include "bbb/path.h"
#include <string.h>

// -----------------------------------------------------------------------------
// module local variables
// -----------------------------------------------------------------------------

static char current_path[2048] = "/";
static char full_path[2048 + 256];

// -----------------------------------------------------------------------------
// public methods
// -----------------------------------------------------------------------------

const char* path_purify(char* path)
{
    // first eleminate ending slash
    int l = strlen(path) - 1;
    while (l > 0) {
        if (path[l] != '/') break;
        path[l--] = '\0';
    }

    // second eliminate "/.."
    while (1) {
        char* p = strstr(path, "/..");
        if (p == 0) break;
        *p      = 0;
        char* s = strrchr(path, '/');
        s       = (s == 0) ? path : s;
        strcpy(s, p + 3);
    }

    // third eliminate "/./"
    while (1) {
        char* p = strstr(path, "/./");
        if (p == 0) break;
        strcpy(p, p + 2);
    }

    // fourth eliminate "//"
    while (1) {
        char* p = strstr(path, "//");
        if (p == 0) break;
        strcpy(p, p + 1);
    }
    return path;
}

// -----------------------------------------------------------------------------

const char* path_set_current_dir(const char* path)
{
    if (*path == '/') {
        strncpy(current_path, path, sizeof(current_path) - 1);
    } else {
        strncat(
            current_path, "/", sizeof(current_path) - strlen(current_path) - 1);
        strncat(current_path,
                path,
                sizeof(current_path) - strlen(current_path) - 1);
    }
    current_path[sizeof(current_path) - 1] = 0;

    return path_purify(current_path);
}

// -----------------------------------------------------------------------------

const char* path_get_current_dir() { return current_path; }

// -----------------------------------------------------------------------------

const char* path_build_full(const char* path)
{
    if (*path == '/') {
        strncpy(full_path, path, sizeof(full_path) - 1);
    } else {
        strcpy(full_path, current_path);
        strncat(full_path, "/", sizeof(full_path) - strlen(full_path) - 1);
        strncat(full_path, path, sizeof(full_path) - strlen(full_path) - 1);
    }
    full_path[sizeof(full_path) - 1] = 0;

    return path_purify(full_path);
}

// -----------------------------------------------------------------------------

const char* path_build_full_r(char* buf, size_t buf_sz, const char* path)
{
    if (*path == '/') {
        strncpy(buf, path, buf_sz - 1);
    } else {
        strcpy(buf, current_path);
        strncat(buf, "/", buf_sz - strlen(buf) - 1);
        strncat(buf, path, buf_sz - strlen(buf) - 1);
    }
    buf[buf_sz - 1] = 0;

    return path_purify(buf);
}

// -----------------------------------------------------------------------------
// test infrastructure
// -----------------------------------------------------------------------------

#ifdef __TEST_MODULE__
#include <stdio.h>

#define ARRAY_SIZE(x) (sizeof(x) / sizeof(x[0]))

static void printd(const char* s)
{
    while (*s != 0) putchar(*s++);
}

int main()
{
    char t[][100] = {"/aaa/bbb/ccc/ddd",
                     "/aaa/./bbb///ccc/./ddd/"
                     "/aaa/../bbb/ccc",
                     "aaa/../bbb/ccc",
                     "/",
                     "//////////////",
                     "a//////////////"};

    for (unsigned i = 0; i < ARRAY_SIZE(t); i++) {
        printf("\nt%d: ", i);
        printd(t[i]);
        printf("\n  : ");
        printd(path_purify(t[i]));
    }
    printf("\n");
    return 0;
}
#endif
