#include <stdarg.h>
#include <stddef.h>
#include "stivale2.h"
#include "framebuffer.h"

#define SSFN_CONSOLEBITMAP_TRUECOLOR
#define SSFN_NOIMPLEMENTATION
#include "ssfn.h"

extern uint8_t _binary_sfn_fonts_unifont_sfn_start;

struct GFX_Struct gfx;

void framebuffer_init(struct stivale2_struct *stivale2_struct, uint32_t background_color)
{
	struct stivale2_struct_tag_framebuffer *framebuffer_info = stivale2_get_tag(stivale2_struct,
			STIVALE2_STRUCT_TAG_FRAMEBUFFER_ID);

	gfx.fb_addr		= framebuffer_info->framebuffer_addr;
	gfx.fb_width	= framebuffer_info->framebuffer_width;
	gfx.fb_height	= framebuffer_info->framebuffer_height;
	gfx.fb_pitch	= framebuffer_info->framebuffer_pitch;
	gfx.fb_bpp		= framebuffer_info->framebuffer_bpp;
	gfx.glyph_width	= 8;
	gfx.glyph_height	= 16;

	ssfn_src		= (ssfn_font_t *)&_binary_sfn_fonts_unifont_sfn_start;
	ssfn_dst.ptr	= (uint8_t *)gfx.fb_addr;
	ssfn_dst.w		= gfx.fb_width;
	ssfn_dst.h		= gfx.fb_height;
	ssfn_dst.p		= gfx.fb_pitch;
	ssfn_dst.x		= 0;
	ssfn_dst.y		= 0;

	framebuffer_set_background_color(background_color);
}

void framebuffer_draw_pixel(int x, int y, uint32_t color)
{
	size_t fb_index = y * (gfx.fb_pitch / sizeof(uint32_t)) + x;
	uint32_t *fb = (uint32_t *)gfx.fb_addr;

	fb[fb_index] = color;
}

void framebuffer_set_background_color(uint32_t background_color)
{
	ssfn_dst.bg = background_color;

	for (int y = 0; y < gfx.fb_height; y++)
	{
		for (int x = 0; x < gfx.fb_width; x++)
			framebuffer_draw_pixel(x, y, ssfn_dst.bg);
	}
}

void framebuffer_reset_screen(void)
{
	ssfn_dst.x = 0;
	ssfn_dst.y = 0;

	framebuffer_set_background_color(ssfn_dst.bg);
}

void framebuffer_move_one_row_up(void)
{
	uint32_t *fb = (uint32_t *)gfx.fb_addr;

	for (int column = gfx.glyph_height; column < gfx.fb_height; column++)
	{
		for (int row = 0; row < gfx.fb_width; row++)
		{
			size_t current_index = column * (gfx.fb_pitch / sizeof(uint32_t)) + row;
			uint32_t current_color = fb[current_index];
			fb[current_index] = ssfn_dst.bg;

			size_t new_index = (column - gfx.glyph_height) * (gfx.fb_pitch / sizeof(uint32_t)) + row;
			fb[new_index] = current_color;
		}
	}
}

void framebuffer_print_char(uint32_t unicode, int x, int y, uint32_t foreground_color)
{
	ssfn_dst.x = x;
	ssfn_dst.y = y;

	if (ssfn_dst.x + gfx.glyph_width > gfx.fb_width)
	{
		ssfn_dst.x = 0;
		ssfn_dst.y += gfx.glyph_height;
	}

	if (ssfn_dst.y >= gfx.fb_height)
	{
		ssfn_dst.x = 0;
		ssfn_dst.y = gfx.fb_height - gfx.glyph_height;
		framebuffer_move_one_row_up();
	}

	if (unicode == '\t')
	{
		for (int i = 0; i < 4; i++)
			framebuffer_print_string(" ", foreground_color);

		return;
	}

	if (unicode == '\b')
	{
		if (ssfn_dst.x == 0)
		{
			if (ssfn_dst.y == 0)
				return;

			ssfn_dst.x = gfx.fb_width - gfx.glyph_width;
			ssfn_dst.y -= gfx.glyph_height;

			ssfn_putc(' ');

			ssfn_dst.x -= gfx.glyph_width;
		}
		else
		{
			ssfn_dst.x -= gfx.glyph_width;

			ssfn_putc(' ');

			ssfn_dst.x -= gfx.glyph_width;
		}

		return;
	}

	ssfn_dst.fg = foreground_color;

	ssfn_putc(unicode);
}

void framebuffer_print_string(char *string, uint32_t foreground_color)
{
	while (*string)
		framebuffer_print_char(ssfn_utf8(&string), ssfn_dst.x, ssfn_dst.y, foreground_color);
}
