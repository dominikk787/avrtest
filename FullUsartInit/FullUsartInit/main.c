#include <avr/io.h>

enum UsartParity {
	None,
	Even,
	Odd
};

void usartInit(uint32_t speed, enum UsartParity parity, uint8_t dataBits, uint8_t stopBits);

int main(void)
{
    usartInit(9600, None, 8, 1);
    while (1) 
    {
    }
}

void usartInit(uint32_t speed, enum UsartParity parity, uint8_t dataBits, uint8_t stopBits) {
	UCSRB = _BV(RXEN) | _BV(TXEN);
	uint8_t ucsrcValue = 0;
	switch (parity) {
		case None:
			break;
		case Even:
			ucsrcValue |= _BV(UPM1);
			break;
		case Odd:
			ucsrcValue |= _BV(UPM1) | _BV(UPM0);
			break;
	}
	switch (dataBits) {
		case 5:
			break;
		case 6:
			ucsrcValue |= _BV(UCSZ0);
			break;
		case 7:
			ucsrcValue |= _BV(UCSZ1);
			break;
		case 8:
			ucsrcValue |= _BV(UCSZ1) | _BV(UCSZ0);
			break;
		case 9:
			ucsrcValue |= _BV(UCSZ1) | _BV(UCSZ0);
			UCSRB |= _BV(UCSZ2);
			break;
	}
	if (stopBits == 2) ucsrcValue |= _BV(USBS);
	UCSRC = _BV(URSEL) | ucsrcValue;
	UBRRL = F_CPU/(16 * speed) - 1;
}