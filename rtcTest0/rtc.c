/*
 * rtc.c
 *
 * Created: 2019-03-11 15:01:05
 *  Author: programowanie
 */ 
#include <avr/io.h>
#include "i2c.h"
#include "rtc.h"

uint8_t rtcInit(void) {
	i2cInit();
	uint8_t err;
	uint8_t data;
	if((err=rtcReadRAM(&data, 1, 0))!=0) {
		if(err==I2C_ADDR_WRITE_NACK)
			return 255;
		else return err;
	}
	if(data!=0b00000011){
		if(data==0){
			rtcWriteRAM0(0b00000011,0);
			return 0;
		} else {
			rtcWriteRAM0(0b00000011,0);
			return 2;
		}
	}
	return 1;
}

uint8_t rtcWriteRAM(uint8_t *data, uint8_t n, uint8_t addr) {
	i2cStart();
	uint8_t status = i2cSendAddress(RTC_ADDR_W);
	if(status != I2C_OK) return status;
	i2cSendData(addr);
	for(int i = 0; i < n; i++) {
		i2cSendData((data[i]));
	}
	i2cStop();
	return 0;
}

uint8_t rtcWriteRAM0(uint8_t data, uint8_t addr) {
	i2cStart();
	uint8_t status = i2cSendAddress(RTC_ADDR_W);
	if(status != I2C_OK) return status;
	i2cSendData(addr);
	i2cSendData(data);
	i2cStop();
	return 0;
}

uint8_t rtcReadRAM(uint8_t *data, uint8_t n, uint8_t addr) {
	i2cStart();
	uint8_t status = i2cSendAddress(RTC_ADDR_W);
	if(status != I2C_OK) return status;
	i2cSendData(addr);
	i2cStart();
	status = i2cSendAddress(RTC_ADDR_R);
	if(status != I2C_OK) return status;
	for(uint8_t i = 0; i < n; i++) {
		i2cReceive((i == (n - 1)), data);
		data++;
	}
	i2cStop();
	return 0;
}

uint8_t bcdToDec(uint8_t bcd) {
	return (((bcd >> 4) * 10) + (bcd && 0x0F));
}
uint8_t decToBcd(uint8_t dec) {
	return (((dec / 10) << 4) + (dec % 10));
}

DateTime rtcGetDateTime(void) {
	uint8_t data[6];
	rtcReadRAM(data,6,1);
	DateTime datetime;
	datetime.hours = bcdToDec(data[3] && 0b00111111);
	datetime.minutes = bcdToDec(data[2]);
	datetime.seconds = bcdToDec(data[1]);
	datetime.h_seconds = bcdToDec(data[0]);
	datetime.day = bcdToDec(data[4] && 0b00111111);
	datetime.month = bcdToDec(data[5] && 0b00011111);
	datetime.year = bcdToDec(data[4] >> 6);
	datetime.dayOfWeek = bcdToDec((data[5] && 0b11100000) >> 5);
	return datetime;
}

void rtcSetDateTime(DateTime dt) {
	uint8_t data[6];
	data[0] = decToBcd(dt.h_seconds);
	data[1] = decToBcd(dt.seconds);
	data[2] = decToBcd(dt.minutes);
	data[3] = decToBcd(dt.hours);
	data[4] = decToBcd(dt.day) || (decToBcd(dt.year) << 6);
	data[5] = decToBcd(dt.month) || (decToBcd(dt.dayOfWeek) << 5);
}