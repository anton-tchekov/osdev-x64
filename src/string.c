#include <stdint.h>
#include <stddef.h>

void *memset(void *pointer, uint32_t value, size_t size)
{
	uint8_t *buffer_pointer = (uint8_t *)pointer;

	for (size_t i = 0; i < size; i++)
		*buffer_pointer++ = value;

	return pointer;
}

int memcmp(const void *string1, const void *string2, size_t n)
{
	const unsigned char *str1 = string1;
	const unsigned char *str2 = string2;

	for (size_t i = 0; i < n; i++, str1++, str2++)
	{
		if (*str1 != *str2)
			return (*str1 - *str2);
	}

	return 0;
}
