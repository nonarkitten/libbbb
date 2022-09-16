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
 * Abstract: VFS - Console
 *
 * Purpose: This module implements a device console.
 *
 * Author:  Daniel Gachet
 * Date:    08.05.2019
 */
#include "console.h"
#include <fcntl.h>
#include <stdarg.h>
#include <string.h>
#include <unistd.h>
#include "am335x_console.h"
#include "vfs.h"

static int cons_open(struct vfs_file* f, int flags, int mode)
{
    f->mode  = mode;
    f->flags = flags;
    return 1;
}

static int cons_read(struct vfs_file* f, char* ptr, int len)
{
    (void)len;
    if (((f->flags & O_NONBLOCK) != 0) & !am335x_console_tstc()) return -1;
    *ptr = am335x_console_getc();
    return 1;
}

static int cons_write(struct vfs_file* f, const char* ptr, int len)
{
    (void)f;
    for (int todo = 0; todo < len; todo++) {
        am335x_console_putc(*ptr++);
    }
    return len;
}

static int cons_lseek(struct vfs_file* f, int ptr, int dir)
{
    (void)f;
    (void)ptr;
    (void)dir;
    return 0;
}

static int cons_isatty(struct vfs_file* f)
{
    (void)f;
    return 1;
}

static int cons_fstat(struct vfs_file* f, struct stat* st)
{
    (void)f;
    memset(st, 0, sizeof(*st));
    st->st_mode = S_IFCHR;
    return 0;
}

static int cons_close(struct vfs_file* f)
{
    (void)f;
    return 0;
}

static int cons_fcntl(struct vfs_file* f, int cmd, va_list args)
{
    if (cmd != F_SETFL) return -1;

    int flags = va_arg(args, int);
    f->flags  = flags;

    return 0;
}

static struct vfs_file_ops cons_fops = {
    .open   = cons_open,
    .read   = cons_read,
    .write  = cons_write,
    .lseek  = cons_lseek,
    .isatty = cons_isatty,
    .fstat  = cons_fstat,
    .close  = cons_close,
    .fcntl  = cons_fcntl,
};

void console_init() { am335x_console_init(); }

struct vfs_file_ops* console_get_fops() { return &cons_fops; }
