#ifndef PRINTK_H
#define PRINTK_H

#include "framebuffer.h"
#include "serial.h"

#define kernel_log(...) kernel_log_impl(__FILE__, __LINE__, __VA_ARGS__);

typedef enum {
	INFO,
	WARNING,
	ERROR
} STATUS;

void printk(uint32_t foreground_color, char *fmt, ...);
void debug(char *fmt, ...);
void kernel_log_impl(char *file, int line_nr, STATUS status, char *fmt, ...);

#endif
