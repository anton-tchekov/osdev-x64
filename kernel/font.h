#ifndef __FONT_H__
#define __FONT_H__

#include <stdint.h>

enum
{
	CHAR_VISIBLE_SPACE = 128,
	CHAR_TAB_START     = 129,
	CHAR_TAB_MIDDLE    = 130,
	CHAR_TAB_END       = 131,
	CHAR_TAB_BOTH      = 132,
};

typedef struct
{
	int Width;
	int Height;
	const uint8_t *Bitmap;
} Font;

void font_char(int x, int y, int c, int fg, int bg, const Font *font);
void font_str(int x, int y, const char *s, int fg, int bg, const Font *font);

#endif /* __FONT_H__ */
