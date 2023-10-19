#ifndef __MOUSE_H__
#define __MOUSE_H__

#define MOUSE_CLICK_LEFT    0x01
#define MOUSE_CLICK_RIGHT   0x02
#define MOUSE_CLICK_MIDDLE  0x04

/* delta_x, delta_y, buttons */
typedef void (*MouseEvent)(int, int, int);

void mouse_init(void);
void mouse_event_register(MouseEvent handler);

#endif /* __MOUSE_H__ */
