#ifndef __SERIAL_H__
#define __SERIAL_H__

#include <stdint.h>

void serial_init(void);
uint32_t serial_rx(void);
void serial_tx(uint32_t c);
void serial_tx_str(const char *s);

#endif
