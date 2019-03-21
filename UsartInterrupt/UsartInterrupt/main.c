#include <avr/io.h>
#include <avr/interrupt.h>
#include "keyb.h"
#include "usart.h"

int main(void) {
	DDRD = _BV(DDD2);
	keybInit();
	usartInit();
	usartSendString("Test");
	sei();
	//w³¹cz przerwanie
	UCSRB |= _BV(RXCIE);
	while (1) {
		if (readKeyboard()) {
			uint8_t key = getKey();
			if (key < 10) usartSendByte('0' + key);
			if (key == 16) {
				usartSendByte('\r');
				usartSendByte('\n');
			}
		}
	}
}

ISR(USART_RXC_vect) {
	uint8_t value = UDR;
	if (value == 'a') {
		PORTD |= _BV(PD2);
	}
	if (value == 'b') {
		PORTD &= ~_BV(PD2);
	}
}
