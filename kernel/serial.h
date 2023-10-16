#ifndef __SERIAL_H__
#define __SERIAL_H__

#define TERMINAL_BLACK   "\e[0;30m"
#define TERMINAL_RED     "\e[0;31m"
#define TERMINAL_GREEN   "\e[0;32m"
#define TERMINAL_YELLOW  "\e[0;33m"
#define TERMINAL_BLUE    "\e[0;34m"
#define TERMINAL_PURPLE  "\e[0;35m"
#define TERMINAL_CYAN    "\e[0;36m"
#define TERMINAL_WHITE   "\e[0;37m"
#define TERMINAL_RESET   "\e[0m"

void serial_init(void);
int serial_rx(void);
void serial_tx(int c);
void serial_tx_str(const char *s);

#endif
