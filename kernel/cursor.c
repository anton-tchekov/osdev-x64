#include "cursor.h"
#include "graphics.h"
#include <stdio.h>

static const char *cursor_arrow =
	"X           "
	"XX          "
	"X_X         "
	"X__X        "
	"X___X       "
	"X____X      "
	"X_____X     "
	"X______X    "
	"X_______X   "
	"X________X  "
	"X_________X "
	"X__________X"
	"X______XXXXX"
	"X___X__X    "
	"X__X X__X   "
	"X_X  X__X   "
	"XX    X__X  "
	"      X__X  "
	"       XX   ";

#define CURSOR_ARROW_WIDTH  12
#define CURSOR_ARROW_HEIGHT 19

typedef struct
{
	int X, Y;
} Cursor;

static Cursor cursor;

static int limit(int n, int min, int max)
{
	if(n < min)
	{
		return min;
	}

	if(n > max)
	{
		return max;
	}

	return n;
}

static void cursor_draw(void)
{
	graphics_cursor(cursor.X, cursor.Y,
		CURSOR_ARROW_WIDTH, CURSOR_ARROW_HEIGHT, cursor_arrow);
}

void cursor_init(void)
{
	cursor.X = graphics_width() / 2;
	cursor.Y = graphics_height() / 2;
	cursor_draw();
}

void cursor_update(int dx, int dy)
{
	graphics_restore(cursor.X, cursor.Y,
		CURSOR_ARROW_WIDTH, CURSOR_ARROW_HEIGHT);

	cursor.X = limit(cursor.X + dx, 0, graphics_width() - 1);
	cursor.Y = limit(cursor.Y - dy, 0, graphics_height() - 1);

	cursor_draw();
}
