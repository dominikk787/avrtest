#include "usart.h"
#include <util/setbaud.h>

void usartInit(void) {
	UCSR0B = _BV(RXEN0) | _BV(TXEN0);
	UCSR0C = _BV(UCSZ01) | _BV(UCSZ00);
	UBRR0L = UBRRL_VALUE;
}

void usartSendByte(uint8_t value) {
	while (!(UCSR0A & _BV(UDRE0)));
	UDR0 = value;
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

/*void usartInitPrintf(void) {
	fdevopen(&put, NULL);
}

int put(char c, FILE * file) {
	usartSendByte(c);
	return 0;
}*/