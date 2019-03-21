#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>
#include "1wire.h"
#include "usart.h"

#define DS18B20_CONVERT_T 0x44
#define DS18B20_READ_SCRATCHPAD 0xBE

float getTemperature();

struct scratchpad_struct {
	int16_t temperature;
	int16_t temperatureAlarm;
	uint8_t config;
	int8_t reserved[3];
	uint8_t crc;
};

int main(void) {
	usartInit();
	usartInitPrintf();
	while(1){
		float t = getTemperature();
		printf("%.2f\r\n", t);
		_delay_ms(3000);
	}
}

float getTemperature() {
	oneWireReset();
	oneWireSendByte(OW_SKIP_ROM);
	oneWireSendByte(DS18B20_CONVERT_T);
	_delay_ms(750);
	oneWireReset();
	oneWireSendByte(OW_SKIP_ROM);
	oneWireSendByte(DS18B20_READ_SCRATCHPAD);
	struct scratchpad_struct scratchpad;
	uint8_t * byte = (uint8_t *) &scratchpad;
	for (uint8_t i = 0; i < sizeof(scratchpad); i++)
	byte[i] = oneWireReceiveByte();
	return scratchpad.temperature / 16.0;
}
