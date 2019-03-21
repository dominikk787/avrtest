#include <avr/io.h>
#include <util/delay.h>
#include "lcd.h"
#include "1wire.h"

#define DS18B20_CONVERT_T 0x44
#define DS18B20_READ_SCRATCHPAD 0xBE

struct scratchpad_struct {
	int16_t temperature;
	int16_t temperatureAlarm;
	uint8_t config;
	uint8_t reserved[3];
	uint8_t crc;
};

int main(void) {
		DDRB |= _BV(PB1);
		PORTB |= _BV(PB1);
	lcdInit();
	lcdInitPrintf();
	oneWireReset();
	oneWireSendByte(OW_SKIP_ROM);
	oneWireSendByte(DS18B20_CONVERT_T);
	_delay_ms(750);
	oneWireReset();
	oneWireSendByte(OW_SKIP_ROM);
	oneWireSendByte(DS18B20_READ_SCRATCHPAD);
	//deklaracja zmiennej strukturalnej
	struct scratchpad_struct scratchpad;
	//stworzenie wskaünika pokazujπcego na zmiennπ strukturalnπ
	uint8_t * byte = (uint8_t *) &scratchpad;
	//odczyt scratchpada i zapis do zmiennej strukturalnej
	//przez wskaünik bajtowy
	for (uint8_t i = 0; i < sizeof(scratchpad); i++)
	byte[i] = oneWireReceiveByte();
	//obliczenie temperatury
	float t = scratchpad.temperature / 16.0;
	printf("T=%.3f", t);
	//weryfikacja CRC
	uint8_t crc = 0;
	byte = (uint8_t *) &scratchpad;
	for (uint8_t i = 0; i < sizeof(scratchpad) - 1; i++)
	crc = oneWireComputeCRC8(byte[i], crc);
	lcdGotoXY(0, 1);
	printf("CRC: %s", crc == scratchpad.crc ? "OK" : "E");
	PORTB &= ~_BV(PB1);
	while(1) { }
}
