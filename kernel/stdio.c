#include <stdarg.h>
#include <stdint.h>
#include "stivale2.h"
#include "stdio.h"
#include "kprintf.h"
#include "terminal.h"
#include "serial.h"

void printk(const char *fmt, ...)
{
	char buf[512];
	va_list ptr;
	va_start(ptr, fmt);
	vsnprintf(buf, -1, fmt, ptr);
	terminal_print(buf);
	serial_tx_str(buf);
	va_end(ptr);
}
