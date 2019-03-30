/*
 * blink1.c
 *
 * Created: 30.03.2019 15:27:32
 * Author : Dominik
 */ 

#include <avr/io.h>
#include <util/delay.h>

int main(void)
{
    DDRD |= _BV(DDD2);
	PORTD |= _BV(PORTD2);
	_delay_ms(1000);
	PORTD &= ~_BV(PORTD2);
	asm("jmp 0x7e00");
    while (1) 
    {
    }
}

