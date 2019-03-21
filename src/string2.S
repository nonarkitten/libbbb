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
 * Author: 	Daniel Gachet
 * Date: 	28.06.2014
 */

	.global string_copy_long, string_copy_short
	.global string_set_long, string_set_short

	.text
/* 
 * Move memory long word
 * r0 = destination address
 * r1 = source address
 * r2 = number of long words to move
 */
string_copy_long:	
	nop
l1:	ldr	r3, [r1], #4
	str	r3, [r0], #4
	subs	r2, #1
	bpl	l1
	bx lr

/* 
 * Move memory short word
 * r0 = destination address
 * r1 = source address
 * r2 = number of short words to move
 */
string_copy_short:	
	nop
l2:	ldrh	r3, [r1], #2
	strh	r3, [r0], #2
	subs	r2, #1
	bpl	l2
	bx lr

/* 
 * Set memory long word
 * r0 = destination address
 * r1 = value to set
 * r2 = number of long words to set
 */
string_set_long:	
	nop
l3:	str	r1, [r0], #4
	subs	r2, #1
	bpl	l3
	bx lr

/* 
 * Set memory short word
 * r0 = destination address
 * r1 = value to set
 * r2 = number of short words to set
 */
string_set_short:	
	nop
l4:	strh	r1, [r0], #2
	subs	r2, #1
	bpl	l4
	bx lr
