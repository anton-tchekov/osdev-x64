#ifndef __PS2_H__
#define __PS2_H__

#include "keys.h"
#include <stdint.h>

#define MOUSE_CLICK_LEFT    0x01
#define MOUSE_CLICK_RIGHT   0x02
#define MOUSE_CLICK_MIDDLE  0x04

/* key, codepoint, released */
typedef void (*KeyEvent)(uint32_t, uint32_t, uint32_t);

void ps2_init(void);
void keyboard_event_register(KeyEvent handler);

#endif
