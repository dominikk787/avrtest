#include <avr/io.h>
#include <avr/interrupt.h>
#include "keyb.h"
#include "lcd.h"
#include "usart.h"

char buffer[20];
uint8_t bufferIndex = 0;
volatile uint8_t newCommand = 0;

int main(void) {
	DDRD = _BV(DDD2);
	keybInit();
	lcdInit();
	usartInit();
	usartSendString("Test");
	sei();
	//w³¹cz przerwanie
	UCSRB |= _BV(RXCIE);
	while (1) {
		if (readKeyboard()) {
			uint8_t key = getKey();
			if (key < 10) usartSendByte('0' + key);
			if (key == 16) usartSendString("\r\n");
		}
		if (newCommand) {
			newCommand = 0;
			switch (buffer[0]) {
				case 'a':
				PORTD |= _BV(PD2);
				break;
				case 'b':
				PORTD &= ~_BV(PD2);
				break;
				case 't':
				lcdString(buffer + 1);
				break;
			}
		}
	}
}

ISR(USART_RXC_vect) {
	uint8_t value = UDR;
	if (value != '\r') {
		if (bufferIndex < sizeof(buffer)) {
			buffer[bufferIndex++] = value;
		}
	} else {
		buffer[bufferIndex] = 0;
		bufferIndex = 0;
		newCommand = 1;
	}
}
