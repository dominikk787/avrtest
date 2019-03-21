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
				for (uint8_t i = 0; i < buffer[1]; i++) {
					lcdWriteData(buffer[i + 2]);
				}
				break;
			}
		}
	}
}

ISR(USART_RXC_vect) {
	uint8_t value = UDR;
	if (bufferIndex < sizeof(buffer)) {
		buffer[bufferIndex++] = value;
	}
	if (bufferIndex == buffer[1] + 2) {
		buffer[bufferIndex] = 0;
		bufferIndex = 0;
		newCommand = 1;
	}
}
