#include <avr/io.h>

#define OW_PIN PB0
#define OW_DDR DDRB
#define OW_INPUT PINB

#define OW_LOW OW_DDR |= _BV(OW_PIN)
#define OW_HIGH OW_DDR &= ~_BV(OW_PIN)

#define OW_READ_ROM 0x33
#define OW_SKIP_ROM 0xCC
#define OW_SEARCH_ROM 0xF0

uint8_t oneWireReceiveBit();
void oneWireSendByte(uint8_t data);
uint8_t oneWireReceiveByte();
uint8_t oneWireReset();
uint8_t oneWireComputeCRC8(uint8_t inData, uint16_t seed);
int8_t oneWireSearchRom(uint8_t * bitPattern, int8_t lastDeviation);