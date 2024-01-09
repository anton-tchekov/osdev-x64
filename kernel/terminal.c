#include "terminal.h"
#include "graphics.h"
#include "serial.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

typedef struct
{
	uint32_t X;
	uint32_t Y;
	uint32_t BG;
	uint32_t FG;
	uint32_t Width;
	uint32_t Height;
	uint32_t Size;
	uint16_t *Buffer;
} Terminal;

static Terminal terminal;

static uint32_t vga_entry(uint32_t c, uint32_t color)
{
	return c | color << 8;
}

static uint32_t vga_color(uint32_t fg, uint32_t bg)
{
	return fg | bg << 4;
}

void terminal_set_color(uint32_t fg, uint32_t bg)
{
	terminal.FG = fg;
	terminal.BG = bg;
}

void terminal_set_fg(uint32_t fg)
{
	terminal.FG = fg;
}

void terminal_set_bg(uint32_t bg)
{
	terminal.BG = bg;
}

static uint32_t color_lookup(uint32_t c)
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

void terminal_init(uint32_t w, uint32_t h)
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

void terminal_put(uint32_t c, uint32_t fg, uint32_t bg, uint32_t x, uint32_t y)
{
	terminal.Buffer[y * terminal.Width + x] = vga_entry(c, vga_color(fg, bg));
	graphics_char(8 * x, 16 * y, c, color_lookup(fg), color_lookup(bg), 0);
}

static void terminal_cursor(uint32_t color)
{
	if(terminal.X == terminal.Width) return;
	if(terminal.Y == terminal.Height) return;
	graphics_rect(terminal.X * 8, terminal.Y * 16, 2, 16, color_lookup(color));
}

void terminal_char(uint32_t c)
{
	terminal_cursor(terminal.BG);

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
		uint32_t x, y;
		memmove(terminal.Buffer, terminal.Buffer + terminal.Width,
			(terminal.Size - terminal.Width) * sizeof(*terminal.Buffer));

		memset16(terminal.Buffer + terminal.Size - terminal.Width,
			vga_entry(' ', vga_color(terminal.FG, terminal.BG)), terminal.Width);

		for(y = 0; y < terminal.Height; ++y)
		{
			for(x = 0; x < terminal.Width; ++x)
			{
				uint32_t v, l, fg, bg;
				v = terminal.Buffer[y * terminal.Width + x];
				l = v & 0xFF;
				fg = (v >> 8) & 0x0F;
				bg = (v >> 12);
				terminal_put(l, fg, bg, x, y);
			}
		}

		terminal.Y = terminal.Height - 1;
	}

	terminal_cursor(terminal.FG);
}

void terminal_print(const char *s)
{
	uint32_t c;
	while ((c = *s++))
	{
		terminal_char(c);
	}
}
