/*
 * rtcTest1.c
 *
 * Created: 25.03.2019 10:14:26
 * Author : Dominik
 */ 

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "PCF8583.h"
#include "i2c.h"
#include "USART.h"
#include "sin.h"
#include "1wire.h"
#include "lcd.h"

#define decToNum(a, b) ((a * 10) + b)

volatile uint8_t printDT = 0;
volatile uint8_t setDT = 0;
volatile uint8_t debug = 0;

volatile uint8_t godz, min, sek, hsek;
volatile uint8_t dzien, miesiac;
volatile uint16_t rok;

uint16_t dayOfYear(uint8_t day, uint8_t month);
uint16_t minuteOfDay(uint8_t minutes, uint8_t hours);

uint8_t cChHeat[8] = {
	0b11111,
	0b11111,
	0b11111,
	0b11111,
	0b11111,
	0b11111,
	0b11111,
	0b11111
};

volatile uint16_t chargeCycleCount;

int main(void)
{
    initUSART();
    initUSARTstd();
    printString("test ");
	i2cInit();
	PCF8583_init();
	lcdInit();
	lcdDefineChar(cChHeat, 0);
	TCCR0A |= _BV(COM0A1) | _BV(COM0B1) | _BV(WGM00);
	TCCR0B |= _BV(CS00);
	OCR0A = 0;
	OCR0B = 128;
	DDRD |= _BV(DDD5) | _BV(DDD6) | _BV(DDD2);
	UCSR0B |= _BV(RXCIE0);
	MCUCR |= _BV(PUD);
	PORTD |= _BV(PORTD7);
	PCICR |= _BV(PCIE2);
	PCMSK2 |= _BV(PORTD7);
	printString("rtc\r\n");
    while (1) 
    {
		cli();
		PCF8583_get_time((uint8_t *) &godz, (uint8_t *) &min, (uint8_t *) &sek, (uint8_t *) &hsek);
		PCF8583_get_date((uint8_t *) &dzien, (uint8_t *) &miesiac, (uint16_t *) &rok);
		uint8_t daysin = readTable(minuteToTable(minuteOfDay(min, godz)));
		float tempThershold = 20 + (readTable(dayToTable(dayOfYear(dzien, miesiac))) / ((float) 51)) + (daysin / ((float) 255));
		float temp = ds18b20ReadTemp();
		if(temp == 85.0){
			OCR0A = 0;
			PORTD &= ~_BV(PORTD2);
			while((temp = ds18b20ReadTemp())==85.0) {
				sei();
				;
				cli();
			}
		}
		chargeCycleCount = 0;
		DDRD |= _BV(DDD7);
		sei();
		static uint8_t heatstate = 0;
		if(temp < (tempThershold - 0.1)) heatstate = 1;
		if(temp > tempThershold) heatstate = 0;
		char buf0[17];
		char buf1[17];
		sprintf(buf0, "%02d:%02d %02d-%02d",godz,min,dzien,miesiac);
		sprintf(buf1, "Tt=%.2f T=%.2f", tempThershold, temp);
		lcdWriteCommand(LCD_COMMAND_CLEAR);
		lcdString(buf0);
		lcdGotoXY(0, 1);
		lcdString(buf1);
		lcdGotoXY(15, 0);
		if(heatstate) {
			PORTD |= _BV(PORTD2);
			lcdWriteData(0);
		} else {
			PORTD &= ~_BV(PORTD2);
		}
		cli();
		uint16_t capCycles = chargeCycleCount;
		if(capCycles < 470) {
			OCR0A = 128;
		}else {
			OCR0A = 0;
		}
		OCR0B = (daysin >> 1) + 32;
		if(debug) printf("%d,%d\r\n", (uint16_t)(temp * 100), capCycles);
		if(setDT) {
			setDT = 0;
			printString("\r\n");
			PCF8583_set_time(godz, min, sek, hsek);
			PCF8583_set_date(dzien, miesiac, rok);
		}
		if(printDT) {
			printDT = 0;
			printf("%02d:%02d:%02d.%02d %02d-%02d-%04u %0.8f %0.4f %d\r\n", godz, min, sek, hsek, dzien, miesiac, rok, tempThershold, temp, capCycles);
		}
    }
}

ISR(PCINT2_vect) {
	chargeCycleCount++;                                   /* zliczanie cykli */

	DDRD |= (1 << DDD7);                     /* tryb wyjœcia */
	_delay_us(1);                                    /* opóŸnienie ³adowania */

	DDRD &= ~(1 << DDD7);                    /* tryb wejœcia */
	PCIFR |= (1 << PCIF2);            /* wyczyœæ bit przerwania zmiany stanu */
}

ISR(USART_RX_vect) {
	char ch = UDR0;
	static uint8_t chcode = 0;
	static uint8_t dt[25];
	if(chcode == 0) {
		if(ch == 'g') {
			printDT = 1;
		}
		if(ch == 's') {
			chcode = 1;
		}
		if(ch == 'd') {
			debug = !debug;
		}
		if(ch == 'b') {
			asm("jmp 0x7e00");
		}
	}else if(chcode != 0) {
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