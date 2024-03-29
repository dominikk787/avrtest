/*
 * usart.h
 *
 * Created: 2019-06-09 15:43:07
 *  Author: programowanie
 */ 


#ifndef USART_H_
#define USART_H_

/* Functions to initialize, send, receive over USART

   initUSART requires BAUD to be defined in order to calculate
     the bit-rate multiplier.
 */

#ifndef BAUD                          /* if not defined in Makefile... */
#define BAUD  115200                     /* set a safe default baud rate */
#info "BAUD doesn't defined auto-set to 115200"
#endif

                                  /* These are defined for convenience */
#define   USART_HAS_DATA   bit_is_set(UCSR0A, RXC0)
#define   USART_READY      bit_is_set(UCSR0A, UDRE0)

/* Takes the defined BAUD and F_CPU,
   calculates the bit-clock multiplier,
   and configures the hardware USART                   */
void initUSART(void);

/* Blocking transmit and receive functions.
   When you call receiveByte() your program will hang until
   data comes through.  We'll improve on this later. */
void transmitByte(uint8_t data);
uint8_t receiveByte(void);

void printString(const char myString[]);
             /* Utility function to transmit an entire string from RAM */
void readString(char myString[], uint8_t maxLength);
/* Define a string variable, pass it to this function
   The string will contain whatever you typed over serial */

void printByte(uint8_t byte);
                  /* Prints a byte out as its 3-digit ascii equivalent */
void printFloat(float data);
void printWord(uint16_t word);
        /* Prints a word (16-bits) out as its 5-digit ascii equivalent */

void printBinaryByte(uint8_t byte);
                                     /* Prints a byte out in 1s and 0s */
char nibbleToHex(uint8_t nibble);
void printHexByte(uint8_t byte);
                                   /* Prints a byte out in hexadecimal */
uint8_t getNumber(void);
/* takes in up to three ascii digits,
 converts them to a byte when press enter */

void processFormat(char *out, char *format, ...);
void printFormat(char *format, ...);



#endif /* USART_H_ */