#ifndef DS3231_H
#define DS3231_H
#include "main.h"
typedef struct
{
	uint8_t hour;
	uint8_t min;
	uint8_t sec;
	uint8_t day;
	uint8_t date;
	uint8_t mon;
	uint8_t year;
}DateTime;

void rtc_write_time(DateTime *dt);
void rtc_read_time(DateTime *dt);
float rtc_read_temp(DateTime *dt);
#endif