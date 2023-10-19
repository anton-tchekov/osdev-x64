#include "terminal.h"
#include "types.h"
#include "graphics.h"
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

static inline u16 vga_entry(char c, u8 color)
{
	return (u16)c | (u16)color << 8;
}

static inline u8 vga_color(u8 fg, u8 bg)
{
	return fg | bg << 4;
}

void terminal_init(int w, int h)
{
	terminal.X = 0;
	terminal.Y = 0;
	terminal.Width = w / 8;
	terminal.Height = h / 16;
	terminal.Size = terminal.Width * terminal.Height;
	terminal.Buffer = malloc(terminal.Size * sizeof(*terminal.Buffer));
}

void terminal_put(char c, u8 fg, u8 bg, u32 x, u32 y)
{
	terminal.Buffer[y * terminal.Width + x] = vga_entry(c, vga_color(fg, bg));
	graphics_char(8 * x, 16 * y, c, GFX_WHITE, GFX_BLACK, 0);
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
		/* Scroll Up */
		memmove(terminal.Buffer, terminal.Buffer + terminal.Width,
				(terminal.Size - terminal.Width) * sizeof(*terminal.Buffer));

		/* Clear last line */
		memset16(terminal.Buffer + terminal.Size - terminal.Width,
			vga_entry(' ', vga_color(terminal.FG, terminal.BG)), terminal.Width);

		{
			/* FIXME */
			int x, y;
			for(y = 0; y < terminal.Height; ++y)
			{
				for(x = 0; x < terminal.Width; ++x)
				{
					terminal_put(terminal.Buffer[y * terminal.Width + x],
						terminal.FG, terminal.BG, x, y);
				}
			}
		}

		terminal.Y = terminal.Height - 1;
	}

	/*cursor_move(terminal.X, terminal.Y);*/
}

void terminal_print(const char *s)
{
	int c;
	while ((c = *s++))
	{
		terminal_char(c);
	}
}
