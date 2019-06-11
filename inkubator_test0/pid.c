/*
 * pid.c
 *
 * Created: 2019-06-09 16:04:44
 *  Author: programowanie
 */ 

#include <avr/io.h>
#include "pid.h"

uint16_t pid(uint16_t input, uint16_t setpoint) {
	int32_t error = setpoint - input;
	int32_t Pout = (PID_Kp * error) >> 4;
	static int32_t errorsum = 0;
	errorsum += error;
	if(errorsum > 0xFFF) errorsum = 0xFFF;
	if(errorsum < -0xFFF) errorsum = -0xFFF;
	int32_t Iout = (PID_Ki * errorsum) >> 4;
	static int32_t lasterror = 0;
	int32_t derror = error - lasterror;
	int32_t Dout = (PID_Kd * derror) >> 4;
	lasterror = error;
	if(Pout > 0xFFF) Pout = 0xFFF;
	if(Pout < -0xFFF) Pout = -0xFFF;
	if(Iout > 0xFFF) Iout = 0xFFF;
	if(Iout < -0xFFF) Iout = -0xFFF;
	if(Dout > 0xFFF) Dout = 0xFFF;
	if(Dout < -0xFFF) Dout = -0xFFF;
	int32_t out = (Pout + Iout + Dout) >> 1;
	if(out > 0x7FF) out = 0x7FF;
	if(out < -0x7FF) out = -0x7FF;
	out += 0x7FF;
	return out;
}