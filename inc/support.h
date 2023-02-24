/*
    Copyright © 2019 Michal Schulz <michal.schulz@gmx.de>
    https://github.com/michalsc

    This Source Code Form is subject to the terms of the
    Mozilla Public License, v. 2.0. If a copy of the MPL was not distributed
    with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/

#ifndef _SUPPORT_H
#define _SUPPORT_H

#include <stdint.h>
#include <stdarg.h>
#include <stddef.h>

#define TRUE 1
#define FALSE 0

#ifdef __ARMEB__
#warning Compiling in Big Endian Mode!
#define LE16(x) __builtin_bswap16(x)
#define LE32(x) __builtin_bswap32(x)
#define LE64(x) __builtin_bswap64(x)

#define BE16(x) (x)
#define BE32(x) (x)
#define BE64(x) (x)
#else
#warning Compiling in Little Endian Mode!

#define LE16(x) (x)
#define LE32(x) (x)
#define LE64(x) (x)

#define BE16(x) __builtin_bswap16(x)
#define BE32(x) __builtin_bswap32(x)
#define BE64(x) __builtin_bswap64(x)
#endif

static inline uint32_t rd32le(uint32_t* iobase) {
    return LE32(*(volatile uint32_t *)(iobase));
}

static inline uint32_t rd32be(uint32_t* iobase) {
    return BE32(*(volatile uint32_t *)(iobase));
}

static inline uint16_t rd16le(uint32_t* iobase) {
    return LE16(*(volatile uint16_t *)(iobase));
}

static inline uint16_t rd16be(uint32_t* iobase) {
    return BE16(*(volatile uint16_t *)(iobase));
}

static inline uint8_t rd8(uint32_t* iobase) {
    return *(volatile uint8_t *)(iobase);
}

static inline void wr32le(uint32_t* iobase, uint32_t value) {
    *(volatile uint32_t *)(iobase) = LE32(value);
}

static inline void wr32be(uint32_t* iobase, uint32_t value) {
    *(volatile uint32_t *)(iobase) = BE32(value);
}

static inline void wr16le(uint32_t* iobase, uint16_t value) {
    *(volatile uint16_t *)(iobase) = LE16(value);
}

static inline void wr16be(uint32_t* iobase, uint16_t value) {
    *(volatile uint16_t *)(iobase) = BE16(value);
}

static inline void wr8(uint32_t* iobase, uint8_t value) {
    *(volatile uint8_t *)(iobase) = value;
}

#define kprintf printf

void arm_flush_cache(uint32_t* addr, uint32_t length);
void arm_icache_invalidate(uint32_t* addr, uint32_t length);
void arm_dcache_invalidate(uint32_t* addr, uint32_t length);

// const char *remove_path(const char *in);
// size_t strlen(const char *c);
// int strcmp(const char *s1, const char *s2);
// void *memmove(void *dst, const void *src, size_t sz);
// void memcpy(void *dst, const void *src, size_t sz);
// void memset(void *ptr, uint8_t fill, size_t sz);
// void bzero(void *ptr, size_t sz);

#endif /* _SUPPORT_H */
