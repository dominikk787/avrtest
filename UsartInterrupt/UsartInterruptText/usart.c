#include "usart.h"

void usartInit(void) {
	UCSRB = _BV(RXEN) | _BV(TXEN);
	UCSRC = _BV(URSEL) | _BV(UCSZ1) | _BV(UCSZ0);
	UBRRL = F_CPU/(16 * 9600L) - 1;
}

void usartSendByte(uint8_t value) {
	while (!(UCSRA & _BV(UDRE)));
	UDR = value;
}

void usartSendBytes(uint8_t values[], uint16_t size) {
	for (uint16_t i = 0; i < size; i++) {
		usartSendByte(values[i]);
	}
}

void usartSendString(char str[]) {
	uint16_t i = 0;
	while (str[i]) {
		usartSendByte(str[i++]);
	}
}

void usartInitPrintf() {
	fdevopen(&usartPut, NULL);
}

int usartPut(char c, FILE * file) {
	usartSendByte(c);
	return 0;
}