/*
 * blink0.c
 *
 * Created: 24.02.2019 12:30:25
 * Author : Dominik
 */ 
#define F_CPU 8000000UL

#include <avr/io.h>
#include <util/delay.h>

int main(void)
{
	DDRB |= _BV(PB1) | _BV(PB2);
    /* Replace with your application code */
    while (1) 
    {
		PORTB |= _BV(PB1) | _BV(PB2);
		_delay_ms(1000);
		PORTB &= ~(_BV(PB1) | _BV(PB2));
		_delay_ms(1000);
    }
}

