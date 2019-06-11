/*
 * startboot.h
 *
 * Created: 2019-06-03 15:55:54
 *  Author: programowanie
 */ 


#ifndef STARTBOOT_H_
#define STARTBOOT_H_

#define STARTBOOT_PINR PINB
#define STARTBOOT_PIN PINB0

void __attribute__ ((naked)) __attribute__ ((section (".init8"))) startboot(void);

void startboot(void) {
	if(!(STARTBOOT_PINR & _BV(STARTBOOT_PIN))) {
		asm("jmp 0x7e00");
	}
}


#endif /* STARTBOOT_H_ */