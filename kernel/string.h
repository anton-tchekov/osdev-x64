#ifndef __STRING_H__
#define __STRING_H__

void *memset(void *ptr, int value, size_t size);
int memcmp(const void *p1, const void *p2, size_t n);
void memset32(uint32_t *start, uint32_t value, size_t count);

#endif
