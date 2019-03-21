#include <avr/io.h>
#include <stdio.h>

void usartInit(void);
void usartSendByte(uint8_t val);
void usartSendBytes(uint8_t values[], uint16_t size);
void usartSendString(char str[]);
void usartInitPrintf();
int usartPut(char c, FILE * file);