#ifndef __KEYBOARD_H__
#define __KEYBOARD_H__

#include "keys.h"

/* key, codepoint, released */
typedef void (*KeyEvent)(int, int, int);

void keyboard_init(void);
void keyboard_event_register(KeyEvent handler);

#endif /* __KEYBOARD_H__ */
