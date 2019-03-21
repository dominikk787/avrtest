#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "usart.h"
#include "1wire.h"
#include "lcd.h"

#define DS18B20_CONVERT_T 0x44
#define DS18B20_READ_SCRATCHPAD 0xBE

#define maxpwm 16

struct scratchpad_struct {
	int16_t temperature;
	int16_t temperatureAlarm;
	uint8_t config;
	uint8_t reserved[3];
	uint8_t crc;
};

uint8_t cChDegree[8] = {
	0b01111,
	0b01001,
	0b01001,
	0b01111,
	0b00000,
	0b00000,
	0b00000,
	0b00000
};

#define _millis_ticks_per_msec 125
#define _millis_ovf_time 500
#define millis ((TCNT1/_millis_ticks_per_msec)+(_millis_ovf_counter*_millis_ovf_time))
volatile uint32_t _millis_ovf_counter = 0;

ISR(TIMER1_OVF_vect) {
	_millis_ovf_counter++;
}

int main(void) {
	PORTC |= _BV(PC5);
	_delay_ms(500);
	TCCR0A |= _BV(COM0A1) | _BV(COM0B1) | _BV(WGM01) | _BV(WGM00);
	TCCR0B |= _BV(CS00);
	OCR0A = 2 << 4;
	OCR0B = 2 << 4;
	DDRD |= _BV(DDD5) | _BV(DDD6) | _BV(DDD2);
	TCCR1A |= _BV(WGM11) | _BV(WGM10);
	TCCR1B |= _BV(WGM13) | _BV(WGM12);
	OCR1A = 62499;
	TIMSK1 |= _BV(TOIE1);
	TCCR1B |= _BV(CS11) | _BV(CS10);
	sei();
	lcdInit();
	lcdDefineChar(cChDegree, 0);
	lcdString("termostat");
	usartInit();
	usartSendString("Test\r\n");

	/*oneWireReset();
	oneWireSendByte(0x33);
	oneWireReceiveByte();
	uint8_t rom[6];
	for (uint8_t i = 0; i < 6; i++)	rom[i] = oneWireReceiveByte();
	oneWireReceiveByte();
	printf("id: ");
	for (uint8_t i = 0; i < 6; i++) printf("%02X", rom[i]);
	printf("\r\n");*/
	
	oneWireReset();
	oneWireSendByte(OW_SKIP_ROM);
	oneWireSendByte(DS18B20_CONVERT_T);

	while (1) {
		static int8_t pwm = 4;
		static uint8_t buttont = 0;
		static uint8_t lcdBacklightTemp = 0;
		static uint8_t lcdBc = 0;
		
		
		if((!(PINC & _BV(PC5))) && (buttont = 0)) {
			_delay_ms(10);
			DDRD |= _BV(DDD6);
			if(OCR0A == 0) {
				OCR0A = 1 << 4;
			}
			buttont = millis;
		}
		if((buttont) && (millis > (buttont+50))) {
			lcdBc++;
			buttont = millis;
			if((lcdBc%8) == 0) {
				if(lcdBacklightTemp <= maxpwm) lcdBacklightTemp++;
				OCR0A = (lcdBacklightTemp-1) << 4;
			}
		}
		if((PINC & _BV(PC5)) && (buttont!=0)) {
			buttont = 0;
			lcdBacklightTemp = 0;
			lcdBc = 0;
		}
		
		if (USART_HAS_DATA) {
			char ch = UDR0;
			pwm = ch - '0' + 1;
		}
		
		if((pwm > 0) && (pwm <= maxpwm)){
			DDRD |= _BV(DDD6);
			if(!lcdBacklightTemp) OCR0A = (pwm-1) << 4;
			else OCR0A = 255;
			OCR0B = ((pwm - 1) << 3) + 127;
		}else{
			if(!lcdBacklightTemp) {
				DDRD &= ~_BV(DDD6);
				OCR0A = 0;
			}
			else {
				DDRD |= _BV(DDD6);
				OCR0A = 255; 
			}
			OCR0B = 127;
		}
		
		if(oneWireReceiveBit()) {
			oneWireReset();
			oneWireSendByte(OW_SKIP_ROM);
			oneWireSendByte(DS18B20_READ_SCRATCHPAD);
			//deklaracja zmiennej strukturalnej
			struct scratchpad_struct scratchpad;
			//stworzenie wskaŸnika pokazuj¹cego na zmienn¹ strukturaln¹
			uint8_t * byte = (uint8_t *) &scratchpad;
			//odczyt scratchpada i zapis do zmiennej strukturalnej
			//przez wskaŸnik bajtowy
			for (uint8_t i = 0; i < sizeof(scratchpad); i++)
			byte[i] = oneWireReceiveByte();
			//weryfikacja CRC
			int crc = 0;
			byte = (uint8_t *) &scratchpad;
			for (uint8_t i = 0; i < sizeof(scratchpad) - 1; i++)
			crc = oneWireComputeCRC8(byte[i], crc);
			if(crc == scratchpad.crc){
				static uint8_t state = 1;
				//obliczenie temperatury
				float t = scratchpad.temperature / 16.0;
				int16_t temp = ((int32_t)scratchpad.temperature * 100) >> 4;
				char buf[9];
				snprintf(buf, sizeof(buf), "%d\r\n", temp);
				usartSendString(buf);
				snprintf(buf, sizeof(buf), "T=%.3f", t);
				/*usartSendString(buf);
				usartSendByte(0xc2);
				usartSendByte(0xb0);*/
				char buf1[20];
				/*snprintf(buf1, sizeof(buf1), "C t=%lu\r\n", millis);
				usartSendString(buf1);*/
				
				snprintf(buf1, sizeof(buf1),"H=%s", (state ? "On " : "Off"));
				
				lcdWriteCommand(LCD_COMMAND_CLEAR);
				lcdGotoXY(0, 0);
				lcdString(buf);
				lcdWriteData(0);
				lcdString("C");
				
				lcdGotoXY(0, 1);
				lcdString(buf1);
				
				if(t >= 25.0) {
					state = 0;
				}else if(t <= 24.5) {
					state = 1;
					
				}
				
				if(state) 
					PORTD |= _BV(PD2);
				else
					PORTD &= ~_BV(PD2);
			}
			oneWireReset();
			oneWireSendByte(OW_SKIP_ROM);
			oneWireSendByte(DS18B20_CONVERT_T);
		}
		
		
	}
}
