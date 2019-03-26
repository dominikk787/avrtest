/*
 * touchSensorTest0.c
 *
 * Created: 26.03.2019 09:36:12
 * Author : Dominik
 */ 

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include "USART.h"

volatile uint16_t chargeCycleCount;

ISR(PCINT2_vect) {
	chargeCycleCount++;                                   /* zliczanie cykli */

	DDRD |= (1 << DDD7);                     /* tryb wyjœcia */
	_delay_us(1);                                    /* opóŸnienie ³adowania */

	DDRD &= ~(1 << DDD7);                    /* tryb wejœcia */
	PCIFR |= (1 << PCIF2);            /* wyczyœæ bit przerwania zmiany stanu */
}

int main(void)
{
    initUSART();
	printString("==[Sensor pojemnoœciowy]==\r\n");

	MCUCR |= _BV(PUD);
	PORTD |= _BV(PORTD7);
	PCICR |= _BV(PCIE2);
	PCMSK2 |= _BV(PORTD7);
    while (1) 
    {
		chargeCycleCount = 0;                            /* zerowanie licznika */
		DDRD |= (1 << DDD7);  /* zaczynamy z na³adowanym kond. */
		sei();                                 /* w³¹cz przerwania i zliczanie */
		_delay_ms(50);
		cli();                                                       /* gotowe */
		printWord(chargeCycleCount);                             /* do analizy */
		printString("\r\n");
    }
}

