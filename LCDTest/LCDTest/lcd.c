#include "lcd.h"
#include <util/delay.h>

static uint8_t switchedTo4BitMode;
static uint8_t checkBusyFlag;

void lcdWrite(uint8_t data, uint8_t rs, uint8_t checkBusyFlag) {
	if (rs) LCD_RS_HIGH; else LCD_RS_LOW;
	#ifdef BITS4
		LCD_DATA_DDR |= 0xf0;
	#else
		LCD_DATA_DDR = 0xff;
	#endif
	#ifdef BITS4
		LCD_DATA_PORT &= data | 0x0f;
		LCD_DATA_PORT |= data & 0xf0;
	#else
		LCD_DATA_PORT = data;
	#endif
	LCD_E_HIGH;
	_delay_us(1);
	LCD_E_LOW;
	_delay_us(1);
}


void lcdWriteCommand(uint8_t command) {
	if (switchedTo4BitMode){
		_delay_ms(1);
		lcdWrite(command, 0, 1);
		lcdWrite(command << 4, 0, 0);
	}else {
		lcdWrite(command, 0, checkBusyFlag);
	}
}

void lcdWriteData(uint8_t data) {
	if (switchedTo4BitMode){
		_delay_ms(1);
		lcdWrite(data, 1, 1);
		lcdWrite(data << 4, 1, 0);
		}else {
		lcdWrite(data, 1, checkBusyFlag);
	}
}

void lcdInit() {
	switchedTo4BitMode = 0;
	checkBusyFlag = 0;
	LCD_E_DDR |= _BV(LCD_E_PIN);
	LCD_RS_DDR |= _BV(LCD_RS_PIN);
	_delay_ms(15);
	lcdWriteCommand(LCD_COMMAND_FUNCTION_SET | LCD_PARAM_FUNCTION_SET_8BIT);
	_delay_ms(4.1);
	lcdWriteCommand(LCD_COMMAND_FUNCTION_SET | LCD_PARAM_FUNCTION_SET_8BIT);
	_delay_ms(1);
	lcdWriteCommand(LCD_COMMAND_FUNCTION_SET | LCD_PARAM_FUNCTION_SET_8BIT);
	_delay_ms(1);
	#ifdef BITS4
		_delay_ms(1);
		lcdWriteCommand(LCD_COMMAND_FUNCTION_SET);
		_delay_ms(1);
		switchedTo4BitMode = 1;
		checkBusyFlag = 1;
		lcdWriteCommand(LCD_COMMAND_FUNCTION_SET | LCD_PARAM_FUNCTION_SET_2LINES);
	#else
		checkBusyFlag = 1;
		lcdWriteCommand(LCD_COMMAND_FUNCTION_SET | LCD_PARAM_FUNCTION_SET_8BIT | LCD_PARAM_FUNCTION_SET_2LINES);
	#endif
	_delay_ms(1);
	lcdWriteCommand(LCD_COMMAND_ON_OFF);
	_delay_ms(1);
	lcdWriteCommand(LCD_COMMAND_CLEAR);
	_delay_ms(4.9);
	lcdWriteCommand(LCD_COMMAND_ENTRY_MODE_SET | LCD_PARAM_ENTRY_MODE_SET_INCREMENT);
	_delay_ms(1);
	lcdWriteCommand(LCD_COMMAND_ON_OFF | LCD_PARAM_ON_OFF_DISPLAY);
	_delay_ms(1);
}

void lcdString(char str[]) {
	uint8_t i = 0;
	while (str[i] != 0) {
		lcdWriteData(str[i++]);
		_delay_ms(1);
	}
}

void lcdGotoXY(uint8_t x, uint8_t y){
	lcdWriteCommand(LCD_COMMAND_SET_DDRAM_ADDRESS | (x + y * 0x40));
	_delay_ms(1);
}
