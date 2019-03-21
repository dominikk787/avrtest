#include <util/delay.h>
#include "1wire.h"

void oneWireSendZero() {
	OW_LOW;
	_delay_us(60);
	OW_HIGH;
	_delay_us(10);
}

void oneWireSendOne() {
	OW_LOW;
	_delay_us(6);
	OW_HIGH;
	_delay_us(64);
}

uint8_t oneWireReset() {
	uint8_t ret = 0;
	OW_LOW;
	_delay_us(480);
	OW_HIGH;
	_delay_us(70);
	if (!(OW_INPUT & _BV(OW_PIN))) ret = 1;
	_delay_us(410);
	return ret;
}

uint8_t oneWireReceiveBit() {
	uint8_t ret = 0;
	OW_LOW;
	_delay_us(6);
	OW_HIGH;
	_delay_us(9);
	if (OW_INPUT & _BV(OW_PIN)) ret = 1;
	_delay_us(55);
	return ret;
}

void oneWireSendByte(uint8_t data) {
	for (uint8_t i = 0; i < 8; i++) {
		if(data & 1) {
			oneWireSendOne();
		} else {
			oneWireSendZero();
		}
		data >>= 1;
	}
}

uint8_t oneWireReceiveByte() {
	uint8_t data = 0;
	for (uint8_t i = 0; i < 8; i++) {
		data >>= 1;
		if (oneWireReceiveBit()) data |= 0b10000000;
	}
	return data;
}

uint8_t oneWireComputeCRC8(uint8_t inData, uint16_t seed) {
	uint8_t bitsLeft;
	uint8_t temp;

	for (bitsLeft = 8; bitsLeft > 0; bitsLeft--) {
		temp = ((seed ^ inData) & 0x01);
		if (temp == 0) {
			seed >>= 1;
		}
		else {
			seed ^= 0x18;
			seed >>= 1;
			seed |= 0x80;
		}
		inData >>= 1;
	}
	return seed;
}

int8_t oneWireSearchRom(uint8_t * bitPattern, int8_t lastDeviation)
{
	unsigned char currentBit = 1;
	unsigned char newDeviation = 0;
	unsigned char bitMask = 0x01;
	unsigned char bitA;
	unsigned char bitB;

	oneWireReset();
	oneWireSendByte(OW_SEARCH_ROM);
	
	//wykonujemy pêtlê dla wszystkich 64 bitów
	while (currentBit <= 64) {
		//odczyt bitu i bitu odwrotnego
		bitA = oneWireReceiveBit();
		bitB = oneWireReceiveBit();
		if (bitA && bitB) {
			//jeœli mamy dwie jedynki, zwracamy b³¹d
			return -1;
		} else if (bitA ^ bitB) {
			// bity s¹ ró¿ne, kontynuacja z danym bitem
			if (bitA) {
				(*bitPattern) |= bitMask;
			} else {
				(*bitPattern) &= ~bitMask;
			}
		}
		else { // bity s¹ zerami, konieczna jest decyzja w oparciu o poprzedni¹ decyzjê
			// jeœli jesteœmy w miejscu poprzednio podjêtej decyzji, wybieramy 1
			if (currentBit == lastDeviation) {
				(*bitPattern) |= bitMask;
			}
			// jeœli nie podejmowaliœmy tu decyzji, wybieramy 0
			else if (currentBit > lastDeviation) {
				(*bitPattern) &= ~bitMask;
				newDeviation = currentBit;
			}
			// jeœli bie¿¹cy bit to 0, zapamiêtujemy miejsce decyzji
			else if ( !(*bitPattern & bitMask))	{
				newDeviation = currentBit;
			}
			// jeœli bie¿¹cy bit to 1, nie podejmujemy akcji
			else {}
		}
		// wyœlij wybrany bit
		if ((*bitPattern) & bitMask) {
			oneWireSendOne();
		}
		else {
			oneWireSendZero();
		}
		// przejdŸ do kolejnego bitu
		currentBit++;
		// jeœli osi¹gniêty zosta³ koniec bajtu,
		// przejœcie do nastêpnego bajtu
		bitMask <<= 1;
		if (!bitMask)
		{
			bitMask = 0x01;
			bitPattern++;
		}
	}
	return newDeviation;
}