#include <avr/io.h>
#include <stdio.h>

#define   USART_HAS_DATA   bit_is_set(UCSR0A, RXC0)

void usartInit(void);
void usartSendByte(uint8_t val);
void usartSendBytes(uint8_t values[], uint16_t size);
void usartSendString(char str[]);
//void usartInitPrintf(void);
//int put(char c, FILE * file);