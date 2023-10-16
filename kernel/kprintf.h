#ifndef _PRINTF_H_
#define _PRINTF_H_

#include <stdarg.h>
#include <stddef.h>

void _putchar(char character);

#define printf printf_
int printf_(const char* format, ...);

#define sprintf sprintf_
int sprintf_(char* buffer, const char* format, ...);

#define snprintf  snprintf_
#define vsnprintf vsnprintf_
int  snprintf_(char* buffer, size_t count, const char* format, ...);
int vsnprintf_(char* buffer, size_t count, const char* format, va_list va);

#define vprintf vprintf_
int vprintf_(const char* format, va_list va);

int fctprintf(void (*out)(char character, void* arg), void* arg, const char* format, ...);

#endif
