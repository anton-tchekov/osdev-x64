#include "terminal.h"
#include "types.h"
#include "graphics.h"
#include <string.h>
#include <stdlib.h>

typedef struct
{
	int X;
	int Y;
	TerminalColor BG;
	TerminalColor FG;
	int Width;
	int Height;
	int Size;
	uint16_t *Buffer;
} Terminal;

static Terminal terminal;

static inline uint16_t vga_entry(char c, uint8_t color)
{
	return (uint16_t)c | (uint16_t)color << 8;
}

static inline uint8_t vga_color(uint8_t fg, uint8_t bg)
{
	return fg | bg << 4;
}

void terminal_set_color(TerminalColor fg, TerminalColor bg) {
	terminal.FG = fg;
	terminal.BG = bg;
}

uint32_t colorLookup(TerminalColor c) {
	switch (c)
	{
		case TERMINAL_BLACK:
			return 0xFF3B4252;
		case TERMINAL_WHITE:
			return 0xFFE5E9F0;
		case TERMINAL_RED:
			return 0xFFBF616A;
		case TERMINAL_BLUE:
			return 0xFF81A1C1;
		case TERMINAL_GREEN:
			return 0xFFA3BE8C;
		case TERMINAL_CYAN:
			return 0xFF88C0D0;
		case TERMINAL_PURPLE:
			return 0xFFB48EAD;
		case TERMINAL_YELLOW:
			return 0xFFEBCA8B;
		default:
			return 0xFFFFFFFF;
	}
}

void terminal_init(int w, int h)
{
	terminal.X = 0;
	terminal.Y = 0;
	terminal.BG = TERMINAL_BLACK;
	terminal.FG = TERMINAL_WHITE;
	terminal.Width = w / 8;
	terminal.Height = h / 16;
	terminal.Size = terminal.Width * terminal.Height;
	terminal.Buffer = malloc(terminal.Size * sizeof(*terminal.Buffer));
	memset16(terminal.Buffer,
		vga_entry(' ', vga_color(terminal.FG, terminal.BG)),
		terminal.Size);
}

void terminal_put(char c, TerminalColor fg, TerminalColor bg, uint32_t x, uint32_t y)
{
	terminal.Buffer[y * terminal.Width + x] = vga_entry(c, vga_color(fg, bg));
	graphics_char(8 * x, 16 * y, c, colorLookup(fg), colorLookup(bg), 0);
}

void terminal_char(int c)
{
	if(c == '\n')
	{
		terminal.X = 0;
		++terminal.Y;
	}
	else
	{
		terminal_put(c, terminal.FG, terminal.BG, terminal.X, terminal.Y);
		if(++terminal.X == terminal.Width)
		{
			terminal.X = 0;
			++terminal.Y;
		}
	}

	if(terminal.Y >= terminal.Height)
	{
		int x, y;
		memmove(terminal.Buffer, terminal.Buffer + terminal.Width,
				(terminal.Size - terminal.Width) * sizeof(*terminal.Buffer));

		memset16(terminal.Buffer + terminal.Size - terminal.Width,
			vga_entry(' ', vga_color(terminal.FG, terminal.BG)), terminal.Width);

		for(y = 0; y < terminal.Height; ++y)
		{
			for(x = 0; x < terminal.Width; ++x)
			{
				int v, c, fg, bg;
				v = terminal.Buffer[y * terminal.Width + x];
				c = v & 0xFF;
				fg = (v >> 8) & 0x0F;
				bg = (v >> 12);
				terminal_put(c, fg, bg, x, y);
			}
		}

		terminal.Y = terminal.Height - 1;
	}
}

void terminal_print(const char *s)
{
	int c;
	while ((c = *s++))
	{
		terminal_char(c);
	}
}
