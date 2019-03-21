#pragma once
#ifndef STRING2_H
#define STRING2_H
/**
 * Copyright 2014 University of Applied Sciences Western Switzerland / Fribourg
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
 * Project:	EIA-FR / Embedded Systems 1+2 Laboratory
 *
 * Abstract:	String enhancement
 *
 * Purpose:	String enhancement fonctions
 *
 * Author: 	Daniel Gachet
 * Date: 	28.06.2014
 */


#include <stdint.h>

/** 
 * Move memory long word
 *
 *@param dst destination address
 *@param src source address
 *@len   number of long words to move
 */
extern void string_copy_long (void* dst, void* src, uint32_t len);

/** 
 * Move memory short word
 *
 *@param dst destination address
 *@param src source address
 *@len   number of short words to move
 */
extern void string_copy_short (void* dst, void* src, uint32_t len);

/** 
 * Set memory long word
 *
 *@param dst destination address
 *@param value value to set
 *@len   number of long words to set
 */
extern void string_set_long (void* dst, uint32_t value, uint32_t len);	

/** 
 * Set memory short word
 *
 *@param dst destination address
 *@param value value to set
 *@len   number of long words to set
 */
extern void string_set_short (void* dst, uint16_t value, uint32_t len);

#endif

