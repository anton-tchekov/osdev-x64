#include <stdlib.h>

static uint8_t *p;

void allocator_init(uintptr_t start, size_t size)
{
	p = (uint8_t *)start;
    (void)size;
}

void *malloc(size_t size)
{
	return malloc_align(size, 8);
}

void *malloc_align(size_t size, size_t align)
{
	uintptr_t n = (uintptr_t)p;
	uintptr_t remainder = n & (align - 1);
	void *ret;
	if(remainder == 0)
	{
		ret = (void *)n;
		p = ret + size;
		return ret;
	}

	ret = (void *)(n + align - remainder);
	p = ret + size;
	return ret;
}
