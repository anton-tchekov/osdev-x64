#ifndef __CMOS_H__
#define __CMOS_H__

#include <stdint.h>

typedef struct
{
	uint16_t Year;
	uint8_t Month;
	uint8_t Day;
	uint8_t Hour;
	uint8_t Minute;
	uint8_t Second;
	uint8_t Weekday;
} DateTime;

DateTime rtc_read(void);

#endif
