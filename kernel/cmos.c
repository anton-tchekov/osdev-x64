#include "cmos.h"
#include "cpu.h"

#define CMOS_ADDRESS 0x70
#define CMOS_DATA    0x71

typedef struct
{
	uint8_t Year;
	uint8_t Month;
	uint8_t Day;
	uint8_t Hour;
	uint8_t Minute;
	uint8_t Second;
	uint8_t RegisterB;
} RTC_Registers;

static inline int rtc_ready(void)
{
	outb(CMOS_ADDRESS, 0x0A);
	return inb(CMOS_DATA) & 0x80;
}

static inline uint8_t rtc_read_register(uint8_t r)
{
	outb(CMOS_ADDRESS, r);
	return inb(CMOS_DATA);
}

static inline RTC_Registers rtc_read_registers(void)
{
	RTC_Registers r;
	while(rtc_ready()) {}
	r.Year = rtc_read_register(0x09);
	r.Month = rtc_read_register(0x08);
	r.Day = rtc_read_register(0x07);
	r.Hour = rtc_read_register(0x04);
	r.Minute = rtc_read_register(0x02);
	r.Second = rtc_read_register(0x00);
	r.RegisterB = rtc_read_register(0x0B);
	return r;
}

static inline int weekday(int d, int m, int y)
{
	d += m < 3 ? y-- : y - 2;
	return (23 * m / 9 + d + 4 + y / 4 - y / 100 + y / 400) % 7;
}

static inline int bcd_to_binary(int v)
{
	return (v & 0x0F) + ((v >> 4) * 10);
}

static const char *weekday_str(int weekday_id)
{
	static const char *weekday_strs[] =
	{
		"Sunday",
		"Monday",
		"Tuesday",
		"Wednesday",
		"Thursday",
		"Friday",
		"Saturday"
	};

	return weekday_strs[weekday_id];
}

DateTime rtc_read(void)
{
	DateTime result;
	RTC_Registers r = rtc_read_registers();
	if(!(r.RegisterB & 0x04))
	{
		/* Convert BCD to binary */
		r.Year = bcd_to_binary(r.Year);
		r.Month = bcd_to_binary(r.Month);
		r.Day = bcd_to_binary(r.Day);
		r.Hour = ((r.Hour & 0x0F) + (((r.Hour & 0x70) >> 4) * 10)) |
			(r.Hour & 0x80);
		r.Minute = bcd_to_binary(r.Minute);
		r.Second = bcd_to_binary(r.Second);
	}

	if(!(r.RegisterB & 0x02) && (r.Hour & 0x80))
	{
		/* Convert 12 to 24-hour clock */
		r.Hour = ((r.Hour & 0x7F) + 12) % 24;
	}

	/* Calculate 4-digit year */
	result.Year = ((r.Year < 70) ? 2000 : 1900) + r.Year;
	result.Month = r.Month;
	result.Day = r.Day;
	result.Hour = r.Hour;
	result.Minute = r.Minute;
	result.Second = r.Second;
	result.Weekday = weekday(r.Day, r.Month, r.Year);

	printf("%s, %04d-%02d-%02d %02d:%02d:%02d\n",
		weekday_str(result.Weekday),
		result.Year, result.Month, result.Day,
		result.Hour, result.Minute, result.Second);

	return result;
}
