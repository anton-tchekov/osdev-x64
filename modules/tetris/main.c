#include "../../kernel/module.h"
#include "../../kernel/terminal.h"
#include "../../kernel/keys.h"
#include "../modfunc.h"

const uint64_t *fns;

static const char name[] = "Tetris";
static const char author[] = "Anton Tchekov";
static const char desc[] = "Tetris game";

#define FIELD_WIDTH         10
#define FIELD_HEIGHT        20
#define BLOCK_SIZE          20

#define FALL_SPEED_DEFAULT 200
#define FALL_SPEED_FAST     30

#define WINDOW_WIDTH          (FIELD_WIDTH * BLOCK_SIZE + 6 * BLOCK_SIZE)
#define WINDOW_HEIGHT         (FIELD_HEIGHT * BLOCK_SIZE)

#define WINDOW_TITLE          "Tetris"

typedef enum
{
	RUNNING  = 1
} Status;

typedef enum { I, J, L, O, S, T, Z } PieceType;

typedef struct
{
	int X, Y, Rotation;
	PieceType Type;
} Piece;

struct
{
	uint16_t Blocks[4];
	uint32_t Color;
}
const Pieces[] =
{
	{
		/* I */
		{ 0x0F00, 0x2222, 0x00F0, 0x4444 },
		0x00FFFF /* Cyan */
	},
	{
		/* J */
		{ 0x44C0, 0x8E00, 0x6440, 0x0E20 },
		0x0000FF /* Blue */
	},
	{
		/* L */
		{ 0x4460, 0x0E80, 0xC440, 0x2E00 },
		0xFF7F00 /* Orange */
	},
	{
		/* O */
		{ 0xCC00, 0xCC00, 0xCC00, 0xCC00 },
		0xFFFF00 /* Yellow */
	},
	{
		/* S */
		{ 0x06C0, 0x8C40, 0x6C00, 0x4620 },
		0x00FF00 /* Green */
	},
	{
		/* T */
		{ 0x0E40, 0x4C40, 0x4E00, 0x4640 },
		0xFF00FF /* Purple */
	},
	{
		/* Z */
		{ 0x0C60, 0x4C80, 0xC600, 0x2640 },
		0xFF0000 /* Red */
	}
};

/* Current Piece, Next Piece */
Piece cp, np;
Status status;
int ticks, last_ticks = 0, ticks_update = FALL_SPEED_DEFAULT, score = 0;
int field[FIELD_WIDTH * FIELD_HEIGHT];

static void _draw_field(int *field);
static void _clear_field(int *field);
static int _field_get(int *field, int x, int y);
static int _field_rows(int *field);
static void _field_shift(int *field, int row);

static void _draw_grid(void);

static int _new_piece(Piece *p);
static void _draw_piece(Piece *p);
static int _valid_position(int *field, Piece *p);
static void _to_field(int *field, Piece *p);

static int rng_state = 31419526;
static int rand(void)
{
	rng_state ^= rng_state << 13;
	rng_state ^= rng_state >> 17;
	rng_state ^= rng_state << 5;
	return rng_state;
}

void *memset(void *ptr, int value, size_t size)
{
	uint8_t *p = ptr;
	uint8_t *end = p + size;
	while(p < end)
	{
		*p++ = value;
	}

	return ptr;
}

void *memmove(void *dest, const void *src, size_t len)
{
	char *d = dest;
	const char *s = src;
	if(d < s)
	{
		while(len--)
		{
			*d++ = *s++;
		}
	}
	else
	{
		const char *lasts = s + (len - 1);
		char *lastd = d + (len - 1);
		while(len--)
		{
			*lastd-- = *lasts--;
		}
	}

	return dest;
}

static void _draw_field(int *field)
{
	int x, y, v;
	for(y = 0; y < FIELD_HEIGHT; ++y)
	{
		for(x = 0; x < FIELD_WIDTH; ++x)
		{
			if((v = field[y * FIELD_WIDTH + x]))
			{
				--v;
				graphics_rect(x * BLOCK_SIZE, y * BLOCK_SIZE,
					BLOCK_SIZE, BLOCK_SIZE, Pieces[v].Color);
			}
		}
	}
}

static void _clear_field(int *field)
{
	int i;
	for(i = 0; i < FIELD_WIDTH * FIELD_HEIGHT; ++i)
	{
		field[i] = 0;
	}
}

static int _field_get(int *field, int x, int y)
{
	return (x < 0 || x >= FIELD_WIDTH || y < 0 || y >= FIELD_HEIGHT) ? -1 :
		field[y * FIELD_WIDTH + x];
}

static int _field_rows(int *field)
{
	int x, y, score;
	score = 0;
	for(y = 0; y < FIELD_HEIGHT; ++y)
	{
		for(x = 0; x < FIELD_WIDTH; ++x)
		{
			if(!field[y * FIELD_WIDTH + x])
			{
				break;
			}
		}

		if(x == FIELD_WIDTH)
		{
			_field_shift(field, y);
			score += 100;
		}
	}

	return score;
}

static void _field_shift(int *field, int row)
{
	int x, y;
	for(y = row; y > 0; --y)
	{
		for(x = 0; x < FIELD_WIDTH; ++x)
		{
			field[y * FIELD_WIDTH + x] =
				field[(y - 1) * FIELD_WIDTH + x];
		}
	}
}

static void _draw_grid(void)
{
	int i;
	for(i = 0; i <= BLOCK_SIZE * FIELD_WIDTH; i += BLOCK_SIZE)
	{
		graphics_rect(i, 0, 1, WINDOW_HEIGHT, 0x00000000);
	}

	for(i = 0; i < BLOCK_SIZE * FIELD_HEIGHT; i += BLOCK_SIZE)
	{
		graphics_rect(0, i, BLOCK_SIZE * FIELD_WIDTH, 1, 0x00000000);
	}
}

static int _new_piece(Piece *p)
{
	static int next;
	static int idx = 6;
	static int bag[7];

	if(idx == 6)
	{
		/* create a new shuffled bag of pieces */
		int i, j, tmp;
		for(i = 0; i < 7; ++i)
		{
			bag[i] = i;
		}

		for(i = 0; i < 7; ++i)
		{
			j = rand() % 7;
			tmp = bag[i];
			bag[i] = bag[j];
			bag[j] = tmp;
		}

		idx = 0;
	}

	p->X = 4;
	p->Y = 0;
	p->Rotation = 0;
	p->Type = next;
	next = bag[idx++];
	return next;
}

static void _draw_piece(Piece *p)
{
	int bit, row, col;
	uint16_t blocks;
	row = 0;
	col = 0;
	blocks = Pieces[p->Type].Blocks[p->Rotation];
	for(bit = 0x8000; bit > 0; bit >>= 1)
	{
		if(blocks & bit)
		{
			graphics_rect((p->X + col) * BLOCK_SIZE,
				(p->Y + row) * BLOCK_SIZE,
				BLOCK_SIZE, BLOCK_SIZE, Pieces[p->Type].Color);
		}

		if(++col == 4)
		{
			col = 0;
			++row;
		}
	}
}

static int _valid_position(int *field, Piece *p)
{
	int bit, row, col;
	uint16_t blocks;
	row = 0;
	col = 0;
	blocks = Pieces[p->Type].Blocks[p->Rotation];
	for(bit = 0x8000; bit > 0; bit >>= 1)
	{
		if((blocks & bit) && _field_get(field, p->X + col, p->Y + row))
		{
			return 1;
		}

		if(++col == 4)
		{
			col = 0;
			++row;
		}
	}

	return 0;
}

static void _to_field(int *field, Piece *p)
{
	int bit, row, col;
	uint16_t blocks;
	row = col = 0;
	blocks = Pieces[p->Type].Blocks[p->Rotation];
	for(bit = 0x8000; bit > 0; bit >>= 1)
	{
		if(blocks & bit)
		{
			field[(p->Y + row) * FIELD_WIDTH + (p->X + col)] =
				p->Type + 1;
		}

		if(++col == 4)
		{
			col = 0;
			++row;
		}
	}
}

static void update(void)
{
	_draw_piece(&np);
	_draw_piece(&cp);
	_draw_field(field);
	_draw_grid();
}

static void event_key(uint32_t key, uint32_t ascii, uint32_t released)
{
	if(released)
	{
		switch(key)
		{
		case KEY_DOWN:
			ticks_update = FALL_SPEED_DEFAULT;
			break;

		default:
			break;
		}
	}
	else
	{
		switch(key)
		{
		case KEY_ESCAPE:
			status &= ~RUNNING;
			break;

		case KEY_DOWN:
			ticks_update = FALL_SPEED_FAST;
			break;

		case KEY_UP:
			if(--cp.Rotation == -1)
			{
				cp.Rotation = 3;
			}

			if(_valid_position(field, &cp))
			{
				if(++cp.Rotation == 4)
				{
					cp.Rotation = 0;
				}
			}
			break;

		case KEY_LEFT:
			--cp.X;
			if(_valid_position(field, &cp))
			{
				++cp.X;
			}
			break;

		case KEY_RIGHT:
			++cp.X;
			if(_valid_position(field, &cp))
			{
				--cp.X;
			}
			break;

		default:
			break;
		}
	}

	update();

	(void)ascii;
}

static void timer_event(void)
{
	/*if(ticks > last_ticks + ticks_update)
	{
		last_ticks = ticks;*/
		++cp.Y;
		if(_valid_position(field, &cp))
		{
			--cp.Y;
			_to_field(field, &cp);
			score += _field_rows(field);
			np.Type = _new_piece(&cp);
			ticks_update = FALL_SPEED_DEFAULT;
			if(_valid_position(field, &cp))
			{
				_clear_field(field);
			}
		}
	/*}

	++ticks;*/
	//printf("UR GAE\n");

	graphics_rect(0, 0, 100, 100, 0xFFFFFFFF);
}

static void mmain(void)
{
	printf("TETRIS\n");
	np.X = 11;
	np.Y = 1;
	np.Rotation = 1;

	//_clear_field(field);
	//np.Type = _new_piece(&cp);
	//keyboard_event_register(event_key);
}

static void signal_handler(uint32_t signal_id, void *data)
{
	switch(signal_id)
	{
	case SIGNAL_ID_INIT:
		fns = ((ModuleInit *)data)->Functions;
		mmain();
		break;
	}
}

static const ModuleHeader module_header
	__attribute__((section(".header")))
	__attribute__((__used__)) =
{
	.Magic = MODULE_MAGIC,
	.Type = MODULE_TYPE_EXECUTABLE,
	.Id = MODULE_ID_TEMPLATE,
	.NumSections = 4,
	.Sections =
	{
		{
			.Type = MODULE_SECTION_NAME,
			.Start = (uintptr_t)name
		},
		{
			.Type = MODULE_SECTION_AUTHOR,
			.Start = (uintptr_t)author
		},
		{
			.Type = MODULE_SECTION_DESCRIPTION,
			.Start = (uintptr_t)desc
		},
		{
			.Type = MODULE_SECTION_SIGNAL_HANDLER,
			.Start = (uintptr_t)signal_handler
		},
	}
};
