#include <stdint.h>
#include "stivale2.h"
#include "keyboard.h"
#include "cpu.h"
#include "stdio.h"

static void (*final_handler)(int, int);

static uint32_t standard_keycodes[] =
{
	KEY_UNKNOWN,
	KEY_ESCAPE,
	KEY_1,
	KEY_2,
	KEY_3,
	KEY_4,
	KEY_5,
	KEY_6,
	KEY_7,
	KEY_8,
	KEY_9,
	KEY_0,
	KEY_MINUS,
	KEY_EQUAL,
	KEY_BACKSPACE,
	KEY_TAB,
	KEY_Q,
	KEY_W,
	KEY_E,
	KEY_R,
	KEY_T,
	KEY_Y,
	KEY_U,
	KEY_I,
	KEY_O,
	KEY_P,
	KEY_LEFTBRACKET,
	KEY_RIGHTBRACKET,
	KEY_RETURN,
	KEY_LCTRL,
	KEY_A,
	KEY_S,
	KEY_D,
	KEY_F,
	KEY_G,
	KEY_H,
	KEY_J,
	KEY_K,
	KEY_L,
	KEY_SEMICOLON,
	KEY_QUOTE,
	KEY_GRAVE,
	KEY_LSHIFT,
	KEY_BACKSLASH,
	KEY_Z,
	KEY_X,
	KEY_C,
	KEY_V,
	KEY_B,
	KEY_N,
	KEY_M,
	KEY_COMMA,
	KEY_DOT,
	KEY_SLASH,
	KEY_RSHIFT,
	KEY_KP_ASTERISK,
	KEY_RALT,
	KEY_SPACE,
	KEY_CAPSLOCK,
	KEY_F1,
	KEY_F2,
	KEY_F3,
	KEY_F4,
	KEY_F5,
	KEY_F6,
	KEY_F7,
	KEY_F8,
	KEY_F9,
	KEY_F10,
	KEY_KP_NUMLOCK,
	KEY_SCROLLLOCK,
	KEY_HOME,
	KEY_KP_8,
	KEY_PAGEUP,
	KEY_KP_2,
	KEY_KP_3,
	KEY_KP_0,
	KEY_KP_DECIMAL,
	KEY_UNKNOWN,
	KEY_UNKNOWN,
	KEY_UNKNOWN,
	KEY_F11,
	KEY_F12
};

static uint8_t ctrl		= 0;
static uint8_t alt		= 0;
static uint8_t shift		= 0;

static uint8_t capslock		= 0;
static uint8_t numlock		= 0;
static uint8_t scrolllock   = 0;

void keyboard_init(void)
{
	pic_clear_mask(1);
	while(inb(KB_CONTROLLER_COMMAND) & 0x1)
	{
		inb(KB_CONTROLLER_DATA);
	}

	keyboard_send_command(0xF4);
	kernel_log(INFO, "Keyboard driver initialized\n");
}

void keyboard_send_command(uint8_t command_byte)
{
	while(inb(KB_CONTROLLER_COMMAND) & 0x2)
	{
		outb(KB_CONTROLLER_DATA, command_byte);
	}
}

void keyboard_irq_handler(void)
{
	uint8_t scancode = inb(KB_CONTROLLER_DATA);
	KEY_INFO_t key_info;

	key_info.keycode		= KEY_UNKNOWN;
	key_info.ascii_character	= '\0';

	if (scancode & 0x80)
	{
		scancode -= 0x80;

		uint32_t key = standard_keycodes[scancode];

		if (key == KEY_LCTRL || key == KEY_RCTRL)
			ctrl = 0;
		else if (key == KEY_LALT || key == KEY_RALT)
			alt = 0;
		else if (key == KEY_LSHIFT || key == KEY_RSHIFT)
			shift = 0;
	}
	else
	{
		uint32_t key = standard_keycodes[scancode];

		key_info.keycode = key;

		if (key == KEY_LCTRL || key == KEY_RCTRL)
			ctrl = 1;
		else if (key == KEY_LALT || key == KEY_RALT)
			alt = 1;
		else if (key == KEY_LSHIFT || key == KEY_RSHIFT)
			shift = 1;
		else if (key == KEY_CAPSLOCK)
			capslock = capslock ? 0 : 1;
		else if (key == KEY_KP_NUMLOCK)
			numlock = numlock ? 0 : 1;
		else if (key == KEY_SCROLLLOCK)
			scrolllock = scrolllock ? 0 : 1;

		else if (key <= 0x7F)
			key_info.ascii_character = keycode_to_ascii(key);
	}

	final_handler(key_info.keycode, key_info.ascii_character);
}

char keycode_to_ascii(KEYCODE_t keycode)
{
	uint8_t character = keycode;

	if (shift && capslock)
	{
		if (character == '0')
			character = KEY_RIGHTPARENTHESIS;
		else if (character == '1')
			character = KEY_EXCLAMATION;
		else if (character == '2')
			character = KEY_AT;
		else if (character == '3')
			character = KEY_HASH;
		else if (character == '4')
			character = KEY_DOLLAR;
		else if (character == '5')
			character = KEY_PERCENT;
		else if (character == '6')
			character = KEY_CARRET;
		else if (character == '7')
			character = KEY_AMPERSAND;
		else if (character == '8')
			character = KEY_ASTERISK;
		else if (character == '9')
			character = KEY_LEFTPARENTHESIS;
		else if (character == KEY_COMMA)
			character = KEY_LESS;
		else if (character == KEY_DOT)
			character = KEY_GREATER;
		else if (character == KEY_SLASH)
			character = KEY_QUESTION;
		else if (character == KEY_SEMICOLON)
			character = KEY_COLON;
		else if (character == KEY_QUOTE)
			character = KEY_QUOTEDOUBLE;
		else if (character == KEY_LEFTBRACKET)
			character = KEY_LEFTCURL;
		else if (character == KEY_RIGHTBRACKET)
			character = KEY_RIGHTCURL;
		else if (character == KEY_GRAVE)
			character = KEY_TILDE;
		else if (character == KEY_MINUS)
			character = KEY_UNDERSCORE;
		else if (character == KEY_EQUAL)
			character = KEY_PLUS;
		else if (character == KEY_BACKSLASH)
			character = KEY_BAR;
	}
	else if (shift && !capslock)
	{
		if (character >= 'a' && character <= 'z')
			character -= 32;

		if (character == '0')
			character = KEY_RIGHTPARENTHESIS;
		else if (character == '1')
			character = KEY_EXCLAMATION;
		else if (character == '2')
			character = KEY_AT;
		else if (character == '3')
			character = KEY_HASH;
		else if (character == '4')
			character = KEY_DOLLAR;
		else if (character == '5')
			character = KEY_PERCENT;
		else if (character == '6')
			character = KEY_CARRET;
		else if (character == '7')
			character = KEY_AMPERSAND;
		else if (character == '8')
			character = KEY_ASTERISK;
		else if (character == '9')
			character = KEY_LEFTPARENTHESIS;
		else if (character == KEY_COMMA)
			character = KEY_LESS;
		else if (character == KEY_DOT)
			character = KEY_GREATER;
		else if (character == KEY_SLASH)
			character = KEY_QUESTION;
		else if (character == KEY_SEMICOLON)
			character = KEY_COLON;
		else if (character == KEY_QUOTE)
			character = KEY_QUOTEDOUBLE;
		else if (character == KEY_LEFTBRACKET)
			character = KEY_LEFTCURL;
		else if (character == KEY_RIGHTBRACKET)
			character = KEY_RIGHTCURL;
		else if (character == KEY_GRAVE)
			character = KEY_TILDE;
		else if (character == KEY_MINUS)
			character = KEY_UNDERSCORE;
		else if (character == KEY_EQUAL)
			character = KEY_PLUS;
		else if (character == KEY_BACKSLASH)
			character = KEY_BAR;

	}
	else if (!shift && capslock)
	{
		if (character >= 'a' && character <= 'z')
			character -= 32;
	}

	return character;
}

void activate_keyboard_processing(void *handler)
{
	final_handler = handler;
}
