#include "terminal.h"
#include "graphics.h"
#include "serial.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

typedef struct
{
	int X;
	int Y;
	int BG;
	int FG;
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

void terminal_set_color(int fg, int bg)
{
	terminal.FG = fg;
	terminal.BG = bg;
}

void terminal_set_fg(int fg)
{
	terminal.FG = fg;
}

void terminal_set_bg(int bg)
{
	terminal.BG = bg;
}

static uint32_t color_lookup(int c)
{
	static const uint32_t colors[] =
	{
		GFX_BLACK,
		GFX_RED,
		GFX_GREEN,
		GFX_YELLOW,
		GFX_BLUE,
		GFX_PURPLE,
		GFX_CYAN,
		GFX_WHITE,
	};

	return colors[c];
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

void terminal_put(int c, int fg, int bg, uint32_t x, uint32_t y)
{
	terminal.Buffer[y * terminal.Width + x] = vga_entry(c, vga_color(fg, bg));
	graphics_char(8 * x, 16 * y, c, color_lookup(fg), color_lookup(bg), 0);
}

void terminal_char(int c)
{
	serial_tx(c);
	if(c == '\b')
	{
		if(terminal.X == 0)
		{
			if(terminal.Y > 0)
			{
				--terminal.Y;
			}
		}
		else
		{
			--terminal.X;
		}

		terminal_put(' ', terminal.FG, terminal.BG, terminal.X, terminal.Y);
	}
	else if(c == '\n')
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
