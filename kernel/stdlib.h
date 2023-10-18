#ifndef __STDLIB_H__
#define __STDLIB_H__

#include <stdint.h>
#include <stddef.h>

void allocator_init(uintptr_t start, size_t size);
void *malloc(size_t size);
void *malloc_align(size_t size, size_t align);

#endif
