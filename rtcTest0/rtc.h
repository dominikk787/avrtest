/*
 * rtc.h
 *
 * Created: 2019-03-11 15:01:22
 *  Author: programowanie
 */ 


#ifndef RTC_H_
#define RTC_H_

typedef struct {
	uint8_t h_seconds;
	uint8_t seconds;
	uint8_t minutes;
	uint8_t hours;
	uint8_t dayOfWeek;
	uint8_t day;
	uint8_t month;
	uint8_t year;
} DateTime;

#define RTC_ADDR 0b1010000
#define RTC_ADDR_R ((RTC_ADDR << 1) + 1)
#define RTC_ADDR_W (RTC_ADDR << 1) 
uint8_t rtcInit(void);
uint8_t rtcWriteRAM(uint8_t *data, uint8_t n, uint8_t addr);
uint8_t rtcWriteRAM0(uint8_t data, uint8_t addr);
uint8_t rtcReadRAM(uint8_t *data, uint8_t n, uint8_t addr);
DateTime rtcGetDateTime(void);
void rtcSetDateTime(DateTime dt);

#endif /* RTC_H_ */