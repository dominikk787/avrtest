/*
 * bootloader0.c
 *
 * Created: 2019-03-27 09:31:37
 * Author : programowanie
 */ 

#include <avr/io.h>
#include "boot_opt.h"

#if SPM_PAGESIZE > 255
typedef uint16_t pagelen_t ;
#define GETLENGTH(len) len = getch()<<8; len |= getch()
#else
typedef uint8_t pagelen_t;
#define GETLENGTH(len) (void) getch() /* skip high byte */; len = getch()
#endif

typedef union {
	uint8_t  *bptr;
	uint16_t *wptr;
	uint16_t word;
	uint8_t bytes[2];
} addr16_t;


void __attribute__ ((section (".bootloader"))) startboot(void);
void __attribute__((__noreturn__)) __attribute__ ((section (".bootloader"))) bootloader(void);
void BOOTLOADER_SECTION putch(char ch);
uint8_t BOOTLOADER_SECTION getch(void);
void BOOTLOADER_SECTION verifySpace();
static __attribute__((always_inline)) void writebuffer(addr16_t mybuff, addr16_t address, pagelen_t len);
static __attribute__((always_inline)) void read_mem(addr16_t address, pagelen_t length);

int main(void)
{
	bootloader();
    /* Replace with your application code */
    while (1) 
    {
    }
}

void startboot(void) {
	asm("rjmp boot");
}

void bootloader(void) {
	startboot();
	asm("boot:");
	asm volatile ("clr __zero_reg__");
	UCSR0A |= _BV(U2X0);
	UCSR0B |= _BV(RXEN0) | _BV(TXEN0);
	UCSR0C |= _BV(UCSZ01) | _BV(UCSZ00);
	UBRR0L = 12;
	register addr16_t address;
	register pagelen_t  length;
	addr16_t buff = {(uint8_t *)(RAMSTART)};
	putch('b');
	for(;;) {
		char ch = getch();
		
		if(ch == 'G') {
			unsigned char which = getch();
			verifySpace();
			/*
			* Send optiboot version as "SW version"
			* Note that the references to memory are optimized away.
			*/
			if (which == '1') {
				putch(1);
				putch(0);
			} else if (which == '0') {
				putch(1);
				putch(8);
			} else
				putch(0);
		}
		else if(ch == 'A') {
			// LOAD ADDRESS
			address.bytes[0] = getch();
			address.bytes[1] = getch();
			address.word *= 2; // Convert from word address to byte address
			verifySpace();
			putch(0);
		}
		else if(ch == 'a') {
			// PRINT ADDRESS
			verifySpace();
			putch(2);
			putch(address.bytes[0]);
			putch(address.bytes[1]);
		}
		/* Write memory, length is big endian and is in bytes */
		else if(ch == 'P') {
			// PROGRAM PAGE - we support flash programming only, not EEPROM
			uint8_t *bufPtr;
			pagelen_t savelength;

			GETLENGTH(length);
			savelength = length;

			// read a page worth of contents
			bufPtr = buff.bptr;
			do *bufPtr++ = getch();
			while (--length);

			// Read command terminator, start reply
			verifySpace();
			putch(0);

			writebuffer(buff, address, savelength);


		}
		/* Read memory block mode, length is big endian.  */
		else if(ch == 'R') {
			GETLENGTH(length);

			verifySpace();
			putch(length);

			read_mem(address, length);
		}

		/* Get device signature bytes  */
		else if(ch == 'S') {
			// READ SIGN - return what Avrdude wants to hear
			verifySpace();
			putch(3);
			putch(SIGNATURE_0);
			putch(SIGNATURE_1);
			putch(SIGNATURE_2);
		}
		else if (ch == 'Q') { /* 'Q' */
			// Adaboot no-wait mod
			verifySpace();
			putch(0);
		}
		else {
			// This covers the response to commands like STK_ENTER_PROGMODE
			verifySpace();
			putch(0);
		}
		putch(':');
	}
}

void putch(char ch) {
    while (!(UCSR0A & _BV(UDRE0))) ;

  UDR0 = ch;
}

uint8_t getch(void) {
  while(!(UCSR0A & _BV(RXC0)))  ;

  return UDR0;
}

void verifySpace() {
  if (getch() != ' ') {
    asm ("jmp 0x0000");
  }
  putch(';');
}

/*
 * void writebuffer(memtype, buffer, address, length)
 */
static inline void writebuffer(addr16_t mybuff, addr16_t address, pagelen_t len)
{
	// Copy buffer into programming buffer
	uint16_t addrPtr = address.word;

	/*
	* Start the page erase and wait for it to finish.  There
	* used to be code to do this while receiving the data over
	* the serial link, but the performance improvement was slight,
	* and we needed the space back.
	*/
	__boot_page_erase_short(address.word);
	boot_spm_busy_wait();

	/*
	* Copy data from the buffer into the flash write buffer.
	*/
	do {
	__boot_page_fill_short((uint16_t)(void*)addrPtr, *(mybuff.wptr++));
	addrPtr += 2;
	} while (len -= 2);

	/*
	* Actually Write the buffer to flash (and wait for it to finish.)
	*/
	__boot_page_write_short(address.word);
	boot_spm_busy_wait();
	#if defined(RWWSRE)
	// Reenable read access to flash
	__boot_rww_enable_short();
	#endif
}

static inline void read_mem(addr16_t address, pagelen_t length)
{
    uint8_t ch;

	do {
	    // read a Flash byte and increment the address
	    __asm__ ("lpm %0,Z+\n" : "=r" (ch), "=z" (address.bptr): "1" (address));
	    putch(ch);
	} while (length--);
}