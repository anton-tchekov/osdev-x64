#include "terminal.h"
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

void terminal_init(int w, int h)
{
	terminal.X = 0;
	terminal.Y = 0;
	terminal.Width = w / 8;
	terminal.Height = h / 16;
	terminal.Size = terminal.Width * terminal.Height;
	terminal.Buffer = malloc(terminal.Size * sizeof(*terminal.Buffer));
}

void terminal_char(int c)
{
#if 0
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

		terminal.Y = terminal.Height - 1;
	}

	/*cursor_move(terminal.X, terminal.Y);*/
#endif
}

void terminal_print(const char *s)
{
	int c;
	while ((c = *s++))
	{
		terminal_char(c);
	}
}