#include "graphics.h"
#include <stdarg.h>
#include <stddef.h>
#include <string.h>

typedef struct
{
	uint32_t *Pixels;
	uint16_t Width;
	uint16_t Height;
	uint16_t Pitch;
	uint16_t BPP;
} Graphics;

static Graphics graphics;

void graphics_init(struct stivale2_struct *s, uint32_t color)
{
	struct stivale2_struct_tag_framebuffer *info;

	info = stivale2_get_tag(s, STIVALE2_STRUCT_TAG_FRAMEBUFFER_ID);
	graphics.Pixels = (uint32_t *)info->framebuffer_addr;
	graphics.Width = info->framebuffer_width;
	graphics.Height = info->framebuffer_height;
	graphics.Pitch = info->framebuffer_pitch / sizeof(uint32_t);
	graphics.BPP = info->framebuffer_bpp;
	graphics_clear(color);
}

void graphics_clear(uint32_t color)
{
	uint32_t y;
	uint32_t *offset;

	offset = graphics.Pixels;
	for(y = 0; y < graphics.Height; ++y)
	{
		memset32(offset, color, graphics.Width);
		offset += graphics.Pitch;
	}
}

void graphics_rect(uint32_t x, uint32_t y,
	uint32_t w, uint32_t h, uint32_t color)
{
	uint32_t y_end;
	uint32_t *offset;

	offset = graphics.Pixels + y * graphics.Pitch + x;
	y_end = y + h;
	for(; y < y_end; ++y)
	{
		memset32(offset, color, w);
		offset += graphics.Pitch;
	}
}

void graphics_char(uint32_t x, uint32_t y, uint32_t c,
	uint32_t fg, uint32_t bg, uint32_t font)
{
	/*const uint8_t *char_bitmap = &font->Bitmap[(c - 32) * 16];
	for(int y = 0; y < font->Height; ++y)
	{
		for(int x = 0; x < font->Width; ++x)
		{
			int byte = (y * stride) + (x >> 3);
			int bit = 1 << (7 - (x & 0x7));
			lcd_emit(char_bitmap[byte] & bit ? fg : bg);
		}
	}*/
}

void graphics_string(const char *str, uint32_t foreground_color)
{
}
