#pragma once
#ifndef CONSOLE_H
#define CONSOLE_H
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

/**
 * method to initialize the resources of this module
 */
extern void console_init();

/**
 * method to get the console file operations
 */
extern struct vfs_file_ops* console_get_fops();

#endif
