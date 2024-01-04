#ifndef __TERMINAL_H__
#define __TERMINAL_H__

typedef enum
{
	TERMINAL_BLACK,
	TERMINAL_WHITE,
	TERMINAL_RED,
	TERMINAL_BLUE,
	TERMINAL_GREEN,
	TERMINAL_YELLOW,
	TERMINAL_PURPLE,
	TERMINAL_CYAN
} TerminalColor;

void terminal_set_color(TerminalColor fg, TerminalColor bg);
void terminal_init(int w, int h);
void terminal_print(const char *s);

#endif
