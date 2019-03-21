#include <stdlib.h>
#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>
#include "keyb.h"
#include "lcd.h"

int readNumber() {
	char input[6];
	uint8_t inputIndex = 0;
	while(inputIndex < (sizeof(input) - 1)) {
		uint8_t key = 0;
		key = getKey();
		if (key == 15) {
			if (inputIndex > 0) {
				inputIndex--;
				lcdWriteCommand(LCD_COMMAND_SHIFT);
				printf("%c", ' ');
				lcdWriteCommand(LCD_COMMAND_SHIFT);
			}
			continue;
		}
		if (key < 10) input[inputIndex] = '0' + key;
		if (key == 10) input[inputIndex] = '0';
		if (key == 16) {
			break;
		}
		printf("%c", input[inputIndex]);
		inputIndex++;
	}
	input[inputIndex] = 0;
	return atoi(input);
}

void readSeparatedNumbers(const uint8_t fieldsSizes[], uint8_t fieldsNumber, uint16_t numbers[]) {
	if (!fieldsNumber) return;
	//oblicz d³ugoœæ ci¹gu wejœciowego
	uint8_t inputStringLength = 0;
	for (uint8_t i = 0; i < fieldsNumber; i++) {
		inputStringLength += fieldsSizes[i];
		inputStringLength += 1;
	}
	//oblicz pozycje separatorów
	uint8_t lastSeparatorPosition = 0;
	uint8_t separatorsPositions[fieldsNumber - 1];
	for (uint8_t i = 0; i < sizeof(separatorsPositions); i++) {
		separatorsPositions[i] = lastSeparatorPosition + fieldsSizes[i];
		lastSeparatorPosition = separatorsPositions[i] + 1;
	}
	//wype³nij ci¹g spacjami
	char input[inputStringLength];
	for (uint8_t i = 0; i < sizeof(input); i++) input[i] = ' ';
	uint8_t inputIndex = 0;
	uint8_t key = 0;
	while(1) {
		key = getKey();
		if (key == 15) {
			if (inputIndex > 0) {
				if (inputIndex == sizeof(input) - 2) {
					 printf("%c", ' ');
					 lcdWriteCommand(LCD_COMMAND_SHIFT | LCD_PARAM_SHIFT_LEFT);
				}
				inputIndex--;
				for (uint8_t i = 0; i < sizeof(separatorsPositions); i++) {
					if (inputIndex == separatorsPositions[i]) {
						lcdWriteCommand(LCD_COMMAND_SHIFT | LCD_PARAM_SHIFT_LEFT);
						inputIndex--;
					}
				}
				lcdWriteCommand(LCD_COMMAND_SHIFT | LCD_PARAM_SHIFT_LEFT);
				printf("%c", ' ');
				lcdWriteCommand(LCD_COMMAND_SHIFT | LCD_PARAM_SHIFT_LEFT);
			}
			continue;
		}
		if (key < 10) input[inputIndex] = '0' + key;
		if (key == 10) input[inputIndex] = '0';
		if (key == 16) {
			break;
		}
		printf("%c", input[inputIndex]);
		if (inputIndex < sizeof(input) - 2) {
			inputIndex++;
		} else {
			lcdWriteCommand(LCD_COMMAND_SHIFT | LCD_PARAM_SHIFT_LEFT);
		}
		for (uint8_t i = 0; i < sizeof(separatorsPositions); i++) {
			if (inputIndex == separatorsPositions[i]) {
				lcdWriteCommand(LCD_COMMAND_SHIFT | LCD_PARAM_SHIFT_RIGHT);
				inputIndex++;
			}
		}
	}
	//wpisane ci¹gi cyfr skonwertuj do liczb
	uint8_t offset = 0;
	for (uint8_t i = 0; i < fieldsNumber; i++) {
		numbers[i] = atoi(input + offset);
		offset += fieldsSizes[i] + 1;
	}
}

uint8_t readKeyboard() {
	for (uint8_t row = 0; row < 4; row++){
		KEYB_PORT = ~_BV(row);
		_delay_us(1);
		for (uint8_t column = 0; column < 4; column++){
			if (~KEYB_PIN & _BV(column + 4)) {
				return row * 4 + column + 1;
			}
		}
	}
	return 0;
}

uint8_t getKey() {
	uint8_t key = 0;
	while (!(key = readKeyboard()));
	while (readKeyboard());
	_delay_ms(30);
	return key;
}

void keybInit() {
	KEYB_DDR = 0x0f;
}