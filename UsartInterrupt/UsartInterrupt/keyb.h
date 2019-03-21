#include <avr/io.h>

uint8_t readKeyboard();
uint8_t getKey();
int readNumber();
void keybInit();
void readSeparatedNumbers(const uint8_t fieldsSizes[], uint8_t fieldsNumber, uint16_t numbers[]);

#define KEYB_PORT PORTB
#define KEYB_PIN PINB
#define KEYB_DDR DDRB