#ifndef __WINDOW_H__
#define __WINDOW_H__

#include "types.h"

typedef struct
{
	u16 X, Y, W, H;
	char *Title;
} Window;

void window_draw(Window *window);

#endif
