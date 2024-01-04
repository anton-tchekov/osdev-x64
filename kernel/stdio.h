#ifndef __STDIO_H__
#define __STDIO_H__

#include <stddef.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdbool.h>

void printf(const char *fmt, ...);

#define sprintf sprintf_
int sprintf_(char* buffer, const char* format, ...);

#define snprintf  snprintf_
#define vsnprintf vsnprintf_
int snprintf_(char* buffer, size_t count, const char* format, ...);
int vsnprintf_(char* buffer, size_t count, const char* format, va_list va);

#define vprintf vprintf_
int vprintf_(const char* format, va_list va);

#endif
