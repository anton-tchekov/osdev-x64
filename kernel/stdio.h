#ifndef __STDIO_H__
#define __STDIO_H__

#include <stdint.h>
#include "graphics.h"
#include "serial.h"

#define kernel_log(...) kernel_log_impl(__FILE__, __LINE__, __VA_ARGS__);

typedef enum
{
	INFO,
	WARNING,
	ERROR
} STATUS;

void printk(uint32_t foreground_color, const char *fmt, ...);
void debug(char *fmt, ...);
void kernel_log_impl(char *file, int line_nr, STATUS status, char *fmt, ...);

#endif
