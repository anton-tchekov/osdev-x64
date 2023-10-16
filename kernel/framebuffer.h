#ifndef FRAMEBUFFER_H
#define FRAMEBUFFER_H

#include "stivale2.h"

#define GFX_BLACK	0xFF3B4252
#define GFX_RED		0xFFBF616A
#define GFX_GREEN	0xFFA3BE8C
#define GFX_YELLOW	0xFFEBCA8B
#define GFX_BLUE	0xFF81A1C1
#define GFX_PURPLE	0xFFB48EAD
#define GFX_CYAN	0xFF88C0D0
#define GFX_WHITE	0xFFE5E9F0

struct GFX_Struct
{
	uint64_t fb_addr;
	uint16_t fb_width;
	uint16_t fb_height;
	uint16_t fb_pitch;
	uint16_t fb_bpp;

	int glyph_width;
	int glyph_height;
};

void framebuffer_init(struct stivale2_struct *s, uint32_t background_color);
void framebuffer_draw_pixel(int x, int y, uint32_t color);
void framebuffer_set_background_color(uint32_t background_color);
void framebuffer_reset_screen(void);
void framebuffer_move_one_row_up(void);
void framebuffer_print_char(uint32_t unicode_character, int x, int y, uint32_t foreground_color);
void framebuffer_print_string(char *string, uint32_t foreground_color);

#endif
