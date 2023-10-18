#include "keyboard.h"
#include "cpu.h"
#include "stdio.h"
#include "ctype.h"

static KeyEvent key_event;

static int key_to_codepoint(int k)
{
	int nomods = k & 0xFF;

	if(nomods == KEY_TAB)                             { return '\t'; }
	else if(nomods == KEY_BACKSPACE)                  { return '\b'; }
	else if(nomods == KEY_RETURN)                     { return '\n'; }
	else if(nomods == KEY_SPACE)                      { return ' '; }
	else if(k == (KEY_COMMA | MOD_SHIFT))             { return ';'; }
	else if(k == (KEY_COMMA))                         { return ','; }
	else if(k == (KEY_PERIOD | MOD_SHIFT))            { return ':'; }
	else if(k == (KEY_PERIOD))                        { return '.'; }
	else if(k == (KEY_SLASH | MOD_SHIFT))             { return '_'; }
	else if(k == (KEY_SLASH))                         { return '-'; }
	else if(k == (KEY_BACKSLASH | MOD_SHIFT))         { return '\''; }
	else if(k == (KEY_BACKSLASH))                     { return '#'; }
	else if(k == (KEY_R_BRACKET | MOD_SHIFT))         { return '*'; }
	else if(k == (KEY_R_BRACKET | MOD_ALT_GR))        { return '~'; }
	else if(k == (KEY_R_BRACKET))                     { return '+'; }
	else if(k == (KEY_NON_US_BACKSLASH | MOD_SHIFT))  { return '>'; }
	else if(k == (KEY_NON_US_BACKSLASH | MOD_ALT_GR)) { return '|'; }
	else if(k == KEY_NON_US_BACKSLASH)                { return '<'; }
	else if(k == (KEY_MINUS | MOD_SHIFT))             { return '?'; }
	else if(k == (KEY_MINUS | MOD_ALT_GR))            { return '\\'; }
	else if(k == (KEY_EQUALS | MOD_SHIFT))            { return '`'; }
	else if(k == KEY_GRAVE)                           { return '^'; }
	else if(nomods >= KEY_A && nomods <= KEY_Z)
	{
		int c = nomods - KEY_A + 'a';

		if(c == 'z') { c = 'y'; }
		else if(c == 'y') { c = 'z'; }

		if(k & MOD_ALT_GR)
		{
			if(c == 'q') { return '@'; }
		}

		if(k & MOD_SHIFT)
		{
			c = toupper(c);
		}

		return c;
	}
	else if(nomods >= KEY_1 && nomods <= KEY_0)
	{
		static const char numbers[] =
			{ '1', '2', '3', '4', '5', '6', '7', '8', '9', '0' };

		static const char numbers_shift[] =
			{ '!', '\"', 0, '$', '%', '&', '/', '(', ')', '=' };

		static const char numbers_altgr[] =
			{ 0, 0, 0, 0, 0, 0, '{', '[', ']', '}' };

		int idx = nomods - KEY_1;

		if(k & MOD_SHIFT)
		{
			return numbers_shift[idx];
		}
		else if(k & MOD_ALT_GR)
		{
			return numbers_altgr[idx];
		}
		else
		{
			return numbers[idx];
		}
	}

	return 0;
}

static int scancode_to_key(int scancode, int esc)
{
	if(esc)
	{
		switch(scancode)
		{
			case 0x38: return KEY_R_ALT;
		}
	}
	else
	{
		switch(scancode)
		{
			case 0x01: return KEY_ESCAPE;
			case 0x02: return KEY_1;
			case 0x03: return KEY_2;
			case 0x04: return KEY_3;
			case 0x05: return KEY_4;
			case 0x06: return KEY_5;
			case 0x07: return KEY_6;
			case 0x08: return KEY_7;
			case 0x09: return KEY_8;
			case 0x0A: return KEY_9;
			case 0x0B: return KEY_0;
			case 0x0C: return KEY_MINUS;
			case 0x0D: return KEY_EQUALS;
			case 0x0E: return KEY_BACKSPACE;
			case 0x0F: return KEY_TAB;
			case 0x10: return KEY_Q;
			case 0x11: return KEY_W;
			case 0x12: return KEY_E;
			case 0x13: return KEY_R;
			case 0x14: return KEY_T;
			case 0x15: return KEY_Y;
			case 0x16: return KEY_U;
			case 0x17: return KEY_I;
			case 0x18: return KEY_O;
			case 0x19: return KEY_P;
			case 0x1A: return KEY_L_BRACKET;
			case 0x1B: return KEY_R_BRACKET;
			case 0x1C: return KEY_RETURN;
			case 0x1D: return KEY_L_CTRL;
			case 0x1E: return KEY_A;
			case 0x1F: return KEY_S;
			case 0x20: return KEY_D;
			case 0x21: return KEY_F;
			case 0x22: return KEY_G;
			case 0x23: return KEY_H;
			case 0x24: return KEY_J;
			case 0x25: return KEY_K;
			case 0x26: return KEY_L;
			case 0x27: return KEY_SEMICOLON;
			case 0x29: return KEY_GRAVE;
			case 0x2A: return KEY_L_SHIFT;
			case 0x2B: return KEY_BACKSLASH;
			case 0x2C: return KEY_Z;
			case 0x2D: return KEY_X;
			case 0x2E: return KEY_C;
			case 0x2F: return KEY_V;
			case 0x30: return KEY_B;
			case 0x31: return KEY_N;
			case 0x32: return KEY_M;
			case 0x33: return KEY_COMMA;
			case 0x34: return KEY_PERIOD;
			case 0x35: return KEY_SLASH;
			case 0x36: return KEY_R_SHIFT;
			case 0x38: return KEY_L_ALT;
			case 0x39: return KEY_SPACE;
			case 0x56: return KEY_NON_US_BACKSLASH;
			case 0x5B: return KEY_L_GUI;
		}
	}

	return KEY_UNKNOWN;
}

void keyboard_irq_handler(void)
{
	static int esc, mods;
	int released, key, codepoint, scancode, mod;

	mod = 0;
	scancode = inb(0x60);
	if(scancode == 0xE0)
	{
		esc = 1;
		return;
	}

	released = (scancode >> 7) & 1;
	key = scancode_to_key(scancode & 0x7F, esc);
	if(esc)
	{
		esc = 0;
	}

	if(key == KEY_L_ALT)
	{
		mod = MOD_ALT;
	}
	else if(key == KEY_R_ALT)
	{
		mod = MOD_ALT_GR;
	}
	else if(key == KEY_L_SHIFT || key == KEY_R_SHIFT)
	{
		mod = MOD_SHIFT;
	}
	else if(key == KEY_L_CTRL || key == KEY_R_CTRL)
	{
		mod = MOD_CTRL;
	}
	else if(key == KEY_L_GUI || key == KEY_R_GUI)
	{
		mod = MOD_OS;
	}

	if(mod)
	{
		if(released)
		{
			mods &= ~mod;
		}
		else
		{
			mods |= mod;
		}
	}

	key |= mods;
	codepoint = key_to_codepoint(key);
	if(key_event)
	{
		key_event(key, codepoint, released);
	}
}

void keyboard_init(void)
{
	while(inb(0x64) & 0x01)
	{
		inb(0x60);
	}

	while(inb(0x64) & 0x02)
	{
		outb(0x60, 0xF4);
	}

	isr_register(1, keyboard_irq_handler);
	printk("Keyboard driver initialized\n");
}

void keyboard_event_register(KeyEvent handler)
{
	key_event = handler;
}
