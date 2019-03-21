#include <avr/io.h>
#include "lcd.h"
#include "usart.h"

int main(void) {
	lcdInit();
	lcdInitPrintf();
	lcdWriteCommand(LCD_COMMAND_ON_OFF | LCD_PARAM_ON_OFF_DISPLAY | LCD_PARAM_ON_OFF_CURSOR);
	usartInit();
	char lineBuffer[16];
	for (uint8_t i = 0; i < 16; i++) {
		lineBuffer[i] = ' ';
	}
	uint8_t cursorPosition = 0;
	while (1) {
		if(UCSRA & _BV(RXC)) {
			uint8_t ch = UDR;
			if (ch != '\r') {
				printf("%c", ch);
				lineBuffer[cursorPosition] = ch;
				cursorPosition++;
			}
			if (cursorPosition == 16 || ch == '\r') {
				cursorPosition = 0;
				lcdGotoXY(0, 0);
				for (uint8_t i = 0; i < 16; i++) {
					lcdWriteData(lineBuffer[i]);
				}
				lcdGotoXY(0, 1);
				for (uint8_t i = 0; i < 16; i++) {
					lcdWriteData(' ');
					lineBuffer[i] = ' ';
				}
				lcdGotoXY(0, 1);
			}
		}
	}
}
