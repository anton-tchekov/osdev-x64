#include <string.h>

int memcmp(const void *p1, const void *p2, size_t n)
{
	const uint8_t *s1 = p1;
	const uint8_t *s2 = p2;
	size_t i;
	for(i = 0; i < n; ++i, ++s1, ++s2)
	{
		if(*s1 != *s2)
		{
			return *s1 - *s2;
		}
	}

	return 0;
}

void *memset(void *ptr, int value, size_t size)
{
	uint8_t *p = ptr;
	uint8_t *end = p + size;
	while(p < end)
	{
		*p++ = value;
	}

	return ptr;
}

void memset32(uint32_t *start, uint32_t value, size_t count)
{
	uint32_t *end = start + count;
	while(start < end)
	{
		*start++ = value;
	}
}

void memset16(uint16_t *start, uint16_t value, size_t count)
{
	uint16_t *end = start + count;
	while(start < end)
	{
		*start++ = value;
	}
}

void *memmove(void *dest, const void *src, size_t len)
{
	char *d = dest;
	const char *s = src;
	if(d < s)
	{
		while(len--)
		{
			*d++ = *s++;
		}
	}
	else
	{
		const char *lasts = s + (len - 1);
		char *lastd = d + (len - 1);
		while(len--)
		{
			*lastd-- = *lasts--;
		}
	}

	return dest;
}

int strcmp(const char *s1, const char *s2)
{
	while(*s1 && *s2)
	{
		if(*s1 != *s2)
		{
			return 1;
		}

		++s1;
		++s2;
	}

	return 0;
}
