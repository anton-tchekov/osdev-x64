#include <stdarg.h>
#include <stdint.h>
#include "stivale2.h"
#include "stdio.h"
#include "kprintf.h"
#include "stdio.h"

void printk(uint32_t foreground_color, char *fmt, ...)
{
	char printk_buffer[512];
	va_list ptr;
	va_start(ptr, fmt);
	vsnprintf(printk_buffer, -1, fmt, ptr);
	framebuffer_print_string(printk_buffer, foreground_color);
	va_end(ptr);
}

void debug(char *fmt, ...)
{
	char debug_buffer[512];
	va_list ptr;
	va_start(ptr, fmt);
	vsnprintf(debug_buffer, -1, fmt, ptr);
	serial_tx_str(debug_buffer);
	va_end(ptr);
}

void kernel_log_impl(char *description, int line_nr, STATUS status, char *fmt, ...)
{
	char log_buffer[512];
	va_list ptr;
	va_start(ptr, fmt);
	vsnprintf(log_buffer, -1, fmt, ptr);

	if(status == INFO)
	{
		serial_tx_str(TERMINAL_CYAN);
		debug("[INFO]	| ");
	}
	else if(status == WARNING)
	{
		serial_tx_str(TERMINAL_YELLOW);
		debug("[WARNING] | ");
	}
	else if(status == ERROR)
	{
		serial_tx_str(TERMINAL_RED);
		debug("[ERROR]   | ");
	}

	debug("%s:%d - %s", description, line_nr, log_buffer);
	serial_tx_str(TERMINAL_RESET);

	if(status == INFO)
		printk(GFX_CYAN, "[INFO]      | %s:%d - %s", description, line_nr, log_buffer);
	else if (status == WARNING)
		printk(GFX_YELLOW, "[WARNING] | %s:%d - %s", description, line_nr, log_buffer);
	else if (status == ERROR)
		printk(GFX_RED, "[ERROR]   | %s:%d - %s", description, line_nr, log_buffer);
}
