#include "graphics.h"
#include <stdarg.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>

extern const uint8_t Terminus16[];

typedef struct
{
	uint32_t *Pixels;
	uint32_t *Double;
	uint16_t Width;
	uint16_t Height;
	uint16_t Pitch;
	uint16_t BPP;
} Graphics;

static Graphics graphics;

int graphics_width(void)
{
	return graphics.Width;
}

int graphics_height(void)
{
	return graphics.Height;
}

void graphics_init(struct stivale2_struct *s, uint32_t color)
{
	struct stivale2_struct_tag_framebuffer *info;

	info = stivale2_get_tag(s, STIVALE2_STRUCT_TAG_FRAMEBUFFER_ID);
	graphics.Pixels = (uint32_t *)info->framebuffer_addr;
	graphics.Width = info->framebuffer_width;
	graphics.Height = info->framebuffer_height;
	graphics.Pitch = info->framebuffer_pitch / sizeof(uint32_t);
	graphics.BPP = info->framebuffer_bpp;
	graphics.Double = malloc(
		graphics.Pitch * graphics.Height * sizeof(*graphics.Double));

	graphics_clear(color);
}

void graphics_clear(uint32_t color)
{
	graphics_rect(0, 0, graphics.Width, graphics.Height, color);
}

void graphics_rect(uint32_t x, uint32_t y,
	uint32_t w, uint32_t h, uint32_t color)
{
	uint32_t y_end;
	size_t offset;
	uint32_t *fb, *dbl;

	offset = y * graphics.Pitch + x;
	fb = graphics.Pixels + offset;
	dbl = graphics.Double + offset;
	y_end = y + h;
	for(; y < y_end; ++y)
	{
		memset32(fb, color, w);
		memset32(dbl, color, w);
		fb += graphics.Pitch;
		dbl += graphics.Pitch;
	}
}

void graphics_char(uint32_t x, uint32_t y, uint32_t c,
	uint32_t fg, uint32_t bg, uint32_t font)
{
	const uint8_t *char_bitmap = Terminus16 + (c - 32) * 16;
	size_t offset;
	uint32_t *fb, *dbl;
	uint32_t color;

	offset = y * graphics.Pitch + x;
	fb = graphics.Pixels + offset;
	dbl = graphics.Double + offset;
	int xc, yc, byte;
	for(yc = 0; yc < 16; ++yc, ++char_bitmap)
	{
		byte = *char_bitmap;
		for(xc = 7; xc >= 0; --xc, byte >>= 1)
		{
			color = (byte & 1) ? fg : bg;
			fb[xc] = color;
			dbl[xc] = color;
		}

		fb += graphics.Pitch;
		dbl += graphics.Pitch;
	}

	(void)font;
}
