#include <avr/io.h>
#include <util/delay.h>
#include "lcd.h"

int main(void)
{
	DDRB |= _BV(PB1) | _BV(PB2);
	PORTB |= _BV(PB1) | _BV(PB2);
	lcdInit();
	_delay_ms(1000);
    lcdWriteData('E');
	lcdWriteData('d');
	lcdWriteData('W');
	lcdWriteData(' ');
	lcdString("Elektronika");
	lcdGotoXY(0, 1);
	lcdString("dla wszystkich");
	_delay_ms(1000);
	PORTB &= ~(_BV(PB1) | _BV(PB2));
    while (1) 
    {
    }
}

