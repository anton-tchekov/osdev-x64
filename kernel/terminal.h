#ifndef __TERMINAL_H__
#define __TERMINAL_H__

#include <stdint.h>

enum
{
	TERMINAL_BLACK,
	TERMINAL_RED,
	TERMINAL_GREEN,
	TERMINAL_YELLOW,
	TERMINAL_BLUE,
	TERMINAL_PURPLE,
	TERMINAL_CYAN,
	TERMINAL_WHITE
};

void terminal_set_color(uint32_t fg, uint32_t bg);
void terminal_set_fg(uint32_t fg);
void terminal_set_bg(uint32_t bg);
void terminal_init(uint32_t w, uint32_t h);
void terminal_char(uint32_t c);
void terminal_print(const char *s);

#endif
