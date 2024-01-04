#ifndef __TERMINAL_H__
#define __TERMINAL_H__

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

void terminal_set_color(int fg, int bg);
void terminal_set_fg(int fg);
void terminal_set_bg(int bg);
void terminal_init(int w, int h);
void terminal_print(const char *s);

#endif
