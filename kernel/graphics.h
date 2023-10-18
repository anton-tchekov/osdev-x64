#ifndef __GRAPHICS_H__
#define __GRAPHICS_H__

#include "stivale2.h"
#include "types.h"

#define GFX_BLACK   0xFF3B4252
#define GFX_RED     0xFFBF616A
#define GFX_GREEN   0xFFA3BE8C
#define GFX_YELLOW  0xFFEBCA8B
#define GFX_BLUE    0xFF81A1C1
#define GFX_PURPLE  0xFFB48EAD
#define GFX_CYAN    0xFF88C0D0
#define GFX_WHITE   0xFFE5E9F0

int graphics_width(void);
int graphics_height(void);
void graphics_init(struct stivale2_struct *s, uint32_t color);
void graphics_clear(uint32_t color);
void graphics_rect(uint32_t x, uint32_t y,
	uint32_t w, uint32_t h, uint32_t color);
void graphics_char(uint32_t x, uint32_t y, uint32_t c,
	uint32_t fg, uint32_t bg, uint32_t font);

void graphics_cursor(u32 x, u32 y, u32 w, u32 h, const char *cursor);
void graphics_restore(u32 x, u32 y, u32 w, u32 h);

#endif
