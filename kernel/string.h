#ifndef __STRING_H__
#define __STRING_H__

#include <stdint.h>
#include <stddef.h>

int memcmp(const void *p1, const void *p2, size_t n);
void *memset(void *ptr, int value, size_t size);
void memset16(uint16_t *start, uint16_t value, size_t count);
void memset32(uint32_t *start, uint32_t value, size_t count);
void *memmove(void *dest, const void *src, size_t len);
int strcmp(const char *s1, const char *s2);

#endif
