/*
 * rtcTest0.c
 *
 * Created: 2019-03-11 14:43:02
 * Author : programowanie
 */ 

#include <avr/io.h>
#include "rtc.h"
#include "i2c.h"
#include "USART.h"

int main(void)
{
	initUSART();
	printString("test rtc\r\n");
	printByte(rtcInit());
	uint8_t data0;
	rtcReadRAM(&data0,1,0);
	printBinaryByte(data0);
	printString("\r\n");
    /* Replace with your application code */
    while (1) 
    {
		receiveByte();
		DateTime dt = rtcGetDateTime();
		
		printByte(dt.hours);
		printString(":");
		printByte(dt.minutes);
		printString(":");
		printByte(dt.seconds);
		printString(".");
		printByte(dt.h_seconds);
		printString(" ");
		printByte(dt.day);
		printString("-");
		printByte(dt.month);
		printString("-");
		printByte(dt.year);
		
		printString("\r\n");
    }
}

