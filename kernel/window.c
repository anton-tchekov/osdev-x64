#include "window.h"
#include "graphics.h"

#define BORDER_COLOR GFX_PURPLE
#define BORDER_WIDTH  2

enum
{
	BORDER_TOP    = 0x01,
	BORDER_LEFT   = 0x02,
	BORDER_RIGHT  = 0x04,
	BORDER_BOTTOM = 0x08,
	BORDER_ALL    = 0x0F
};

static void border(u32 x, u32 y, u32 w, u32 h, u32 thickness, u32 which, u32 color)
{
	if(which & BORDER_TOP)
	{
		graphics_rect(x - thickness, y - thickness, w + 2 * thickness, thickness, color);
	}

	if(which & BORDER_LEFT)
	{
		graphics_rect(x - thickness, y, thickness, h, color);
	}

	if(which & BORDER_RIGHT)
	{
		graphics_rect(x + w, y, thickness, h, color);
	}

	if(which & BORDER_BOTTOM)
	{
		graphics_rect(x - thickness, y + h, w + 2 * thickness, thickness, color);
	}
}

void window_border(Window *window)
{
	/* Top */
	//graphics_rect(window->X - 2, window->Y - 22, window->W + 4, 20,
	//	BORDER_COLOR);

	border(window->X, window->Y, window->W, window->H,
		BORDER_WIDTH, BORDER_ALL, BORDER_COLOR);
}

void window_draw(Window *window)
{
	window_border(window);

	graphics_rect(window->X, window->Y, window->W, window->H,
		GFX_CYAN);
}
