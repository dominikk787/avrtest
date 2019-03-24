/*
 * rtcTest0.c
 *
 * Created: 2019-03-11 14:43:02
 * Author : programowanie
 */ 

#include <avr/io.h>
#include <avr/interrupt.h>
#include "rtc.h"
#include "i2c.h"
#include "USART.h"

#define decToNum(a, b) ((a * 10) + b);

volatile uint8_t printDT = 0;
volatile uint8_t setDT = 0;
volatile DateTime DT;

int main(void)
{
	initUSART();
	initUSARTstd();
	printString("test rtc\r\n");
	printByte(rtcInit());
	uint8_t data0;
	rtcReadRAM(&data0,1,0);
	printBinaryByte(data0);
	printString("\r\n");
	UCSR0B |= _BV(RXCIE0);
	sei();
    /* Replace with your application code */
    while (1) 
    {
		if(setDT) {
			//DT = rtcValidateDateTime(DT);
			cli();
			rtcSetDateTime(DT);
			sei();
		}
		if(printDT) {
			printDT = 0;
			DateTime dt = rtcGetDateTime();
			printf("%02d:%02d:%02d.%02d %02d-%02d-%d\r\n", dt.hours, dt.minutes, dt.seconds, dt.h_seconds, dt.day, dt.month, dt.year);
		}
    }
}

ISR(USART_RX_vect) {
	char ch = UDR0;
	static uint8_t chcode = 0;
	static char dt[25];
	if(chcode == 0) {
		if(ch == 'g') {
			printDT = 1;
		}
		if(ch == 's') {
			chcode = 1;
		}
	}
	if(chcode != 0) {
		switch (chcode) {
			case 3:
			case 6:
				if(ch == ':') {
					chcode++;
					transmitByte(ch);
				}
				break;
			case 9:
				if(ch == '.') {
					chcode++;
					transmitByte(ch);
				}
				break;
			case 12:
				if(ch == ' ') {
					chcode++;
					transmitByte(ch);
				}
				break;
			case 15:
			case 18:
				if(ch == '-') {
					chcode++;
					transmitByte(ch);
				}
				break;
			case 20:
				DT.hours = decToNum(dt[0], dt[1]);
				DT.minutes = decToNum(dt[3], dt[4]);
				DT.seconds = decToNum(dt[6], dt[7]);
				DT.h_seconds = decToNum(dt[9], dt[10]);
				DT.day = decToNum(dt[12], dt[13]);
				DT.month = decToNum(dt[15], dt[16]);
				DT.year = dt[17];
				setDT = 1;
				printDT = 1;
				break;
			case 1:
			case 2:
			case 4:
			case 5:
			case 7:
			case 8:
			case 10:
			case 11:
			case 13:
			case 14:
			case 16:
			case 17:
			case 19:
				if(ch >= '0' && ch <= '9') {
					dt[chcode - 1] = ch - '0';
					chcode++;
					transmitByte(ch);
				}
		}
	}
}
