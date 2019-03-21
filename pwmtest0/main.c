/*
 * pwmtest0.c
 *
 * Created: 2019-02-27 10:52:40
 * Author : programowanie
 */ 

#include <avr/io.h>
#include "USART.h"

#define maxpwm 9

int main(void)
{
	TCCR1A |= _BV(COM1A1) | _BV(COM1B1) | _BV(WGM11);
	TCCR1B |= _BV(WGM13) | _BV(WGM12) | _BV(CS10);
	ICR1 = maxpwm;
	DDRB |= _BV(DDB1) | _BV(DDB2);
	OCR1A = 1;
	OCR1B = 1;
	initUSART();
    /* Replace with your application code */
    while (1) 
    {
		char ch = receiveByte();
		int8_t n = ch - '0';
		if((n >= 0) && (n < 10)){
			DDRB |= _BV(DDB1) | _BV(DDB2);
			OCR1A = n;
			OCR1B = n;
		}else{
			DDRB &= ~(_BV(DDB1) | _BV(DDB2));
		}
	}
}

