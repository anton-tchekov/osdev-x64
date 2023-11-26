#ifndef __KEYBOARD_H__
#define __KEYBOARD_H__

#include "keys.h"

#define MOUSE_CLICK_LEFT    0x01
#define MOUSE_CLICK_RIGHT   0x02
#define MOUSE_CLICK_MIDDLE  0x04

/* delta_x, delta_y, buttons */
typedef void (*MouseEvent)(int, int, int);

void mouse_init(void);
void mouse_event_register(MouseEvent handler);

/* key, codepoint, released */
typedef void (*KeyEvent)(int, int, int);

void keyboard_init(void);
void keyboard_event_register(KeyEvent handler);

#endif /* __KEYBOARD_H__ */
