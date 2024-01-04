#ifndef __SERIAL_H__
#define __SERIAL_H__

void serial_init(void);
int serial_rx(void);
void serial_tx(int c);
void serial_tx_str(const char *s);

#endif
