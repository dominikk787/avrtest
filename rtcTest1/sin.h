/*
 * sin.h
 *
 * Created: 25.03.2019 13:35:29
 *  Author: Dominik
 */ 


#ifndef SIN_H_
#define SIN_H_
#include <avr/pgmspace.h>

const uint8_t PROGMEM sintab[180] = {
	0, 0, 0, 0, 0, 0, 1, 1, 1, 2, 2, 2, 3, 3, 4, 4, 5, 6, 6, 7, 8, 8, 9, 
	10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 21, 22, 23, 24, 26, 27, 28, 
	30, 31, 33, 34, 36, 37, 39, 41, 42, 44, 46, 47, 49, 51, 53, 54, 56, 
	58, 60, 62, 64, 66, 68, 70, 72, 74, 76, 78, 80, 82, 84, 86, 88, 90, 
	92, 95, 97, 99, 101, 103, 105, 108, 110, 112, 114, 116, 119, 121, 
	123, 125, 128, 130, 132, 134, 136, 139, 141, 143, 145, 147, 150, 152, 
	154, 156, 158, 160, 163, 165, 167, 169, 171, 173, 175, 177, 179, 181, 
	183, 185, 187, 189, 191, 193, 195, 197, 199, 201, 202, 204, 206, 208, 
	209, 211, 213, 214, 216, 218, 219, 221, 222, 224, 225, 227, 228, 229, 
	231, 232, 233, 234, 236, 237, 238, 239, 240, 241, 242, 243, 244, 245, 
	246, 247, 247, 248, 249, 249, 250, 251, 251, 252, 252, 253, 253, 253, 
	254, 254, 254, 255, 255, 255, 255, 255
};

static uint8_t dayToTable(uint16_t dayOfYear) {
	if(dayOfYear < 3) dayOfYear = 0;
	else dayOfYear -= 3;
	if(dayOfYear > 359) dayOfYear = 359;
	if(dayOfYear < 180) return dayOfYear;
	return 359 - dayOfYear;
}

static uint8_t minuteToTable(uint16_t minuteOfDay) {
	minuteOfDay %= 1440;
	minuteOfDay = minuteOfDay >> 2;
	if(minuteOfDay < 180) return minuteOfDay;
	return 359 - minuteOfDay;
}

static uint8_t readTable(uint8_t addr) {
	return pgm_read_byte(&sintab[addr]);
}

#endif /* SIN_H_ */