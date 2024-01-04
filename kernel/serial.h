#ifndef __SERIAL_H__
#define __SERIAL_H__

#define SERIAL_BLACK   "\e[0;30m"
#define SERIAL_RED     "\e[0;31m"
#define SERIAL_GREEN   "\e[0;32m"
#define SERIAL_YELLOW  "\e[0;33m"
#define SERIAL_BLUE    "\e[0;34m"
#define SERIAL_PURPLE  "\e[0;35m"
#define SERIAL_CYAN    "\e[0;36m"
#define SERIAL_WHITE   "\e[0;37m"
#define SERIAL_RESET   "\e[0m"

void serial_init(void);
int serial_rx(void);
void serial_tx(int c);
void serial_tx_str(const char *s);

#endif
