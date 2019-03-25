/*
 * rtcTest1.c
 *
 * Created: 25.03.2019 10:14:26
 * Author : Dominik
 */ 

#include <avr/io.h>
#include <avr/interrupt.h>
#include "PCF8583.h"
#include "i2c.h"
#include "USART.h"
#include "sin.h"

#define decToNum(a, b) ((a * 10) + b)

volatile uint8_t printDT = 0;
volatile uint8_t setDT = 0;

volatile uint8_t godz, min, sek, hsek;
volatile uint8_t dzien, miesiac;
volatile uint16_t rok;

uint16_t dayOfYear(uint8_t day, uint8_t month);
uint16_t minuteOfDay(uint8_t minutes, uint8_t hours);

int main(void)
{
    initUSART();
    initUSARTstd();
    printString("test rtc\r\n");
	i2cInit();
	PCF8583_init();
	printString("test rtc\r\n");
	UCSR0B |= _BV(RXCIE0);
	sei();
    while (1) 
    {
		cli();
		if(setDT) {
			setDT = 0;
			printString("\r\n");
			PCF8583_set_time(godz, min, sek, hsek);
			PCF8583_set_date(dzien, miesiac, rok);
		}
		PCF8583_get_time((uint8_t *) &godz, (uint8_t *) &min, (uint8_t *) &sek, (uint8_t *) &hsek);
		PCF8583_get_date((uint8_t *) &dzien, (uint8_t *) &miesiac, (uint16_t *) &rok);
		float temp = (readTable(dayToTable(dayOfYear(dzien, miesiac))) / ((float) 51)) + (readTable(dayToTable(minuteOfDay(min, godz))) / ((float) 255));
		if(printDT) {
			printDT = 0;
			printf("%02d:%02d:%02d.%02d %02d-%02d-%04u %0.8f\r\n", godz, min, sek, hsek, dzien, miesiac, rok, temp);
		}
		sei();
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
			case 23:
			godz = decToNum(dt[0], dt[1]);
			min = decToNum(dt[3], dt[4]);
			sek = decToNum(dt[6], dt[7]);
			hsek = decToNum(dt[9], dt[10]);
			dzien = decToNum(dt[12], dt[13]);
			miesiac = decToNum(dt[15], dt[16]);
			rok = (decToNum(dt[18], dt[19]) * 100) + decToNum(dt[20], dt[21]);
			setDT = 1;
			printDT = 1;
			chcode = 0;
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
			case 20:
			case 21:
			case 22:
			if(ch >= '0' && ch <= '9') {
				dt[chcode - 1] = ch - '0';
				chcode++;
				transmitByte(ch);
			}
		}
	}
}

uint16_t dayOfYear(uint8_t day, uint8_t month) {
	uint16_t daysToMonth[12] = {0, 31, 59, 89, 120, 151, 181, 212, 243, 273, 304, 334};
	return day + daysToMonth[month];
}

uint16_t minuteOfDay(uint8_t minutes, uint8_t hours) {
	return minutes + (hours * 60);
}