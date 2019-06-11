/*
 * scheduler_test0.cpp
 *
 * Created: 2019-06-06 12:24:44
 * Author : programowanie
 */ 

#include <avr/io.h>
#include "startboot.h"
#include "scheduler.h"
#include "1wire.h"
#include "dht11.h"
#include "USART.h"

float t = 0;
uint8_t h = 0;

void readt(void);
void readh(void);
void print(void);

SemaphoresData s;
FunctionScheduler sreadt = FunctionScheduler(&s);
FunctionScheduler sreadh = FunctionScheduler(&s);
FunctionScheduler sprint = FunctionScheduler(&s);

int main(void)
{
	sreadt.add(sreadt, SEMAPHORE_NEGATIVE);
	sreadh.add(sreadh, SEMAPHORE_NEGATIVE);
	sprint.add(sreadt, SEMAPHORE_POSITIVE);
	sprint.add(sreadh, SEMAPHORE_POSITIVE);
	initUSART();
	printString("scheduler test\r\n");
    /* Replace with your application code */
    while (1) 
    {
		if(!sreadt.get()) readt();
		if(!sreadh.get()) readh();
		if(sreadt.get() && sreadh.get()) {
			sreadt.reset();
			sreadh.reset();
			print();
		}
    }
}

void readt(void) {
	transmitByte('t');
	static uint8_t c = 0;
	if(c == 0) {
		ds18b20StartTemp();
		c = 1;
		return;
	}
	if(!ds18b20IsReady()) {
		return;
	}
	t = ds18b20GetTemp();
	ds18b20StartTemp();
	sreadt.set();
}

void readh(void) {
	transmitByte('h');
	dht_gethumidity(&h);
	sreadh.set();
}

void print(void) {
	transmitByte('p');
// 	printFloat(t);
// 	transmitByte(' ');
// 	printByte(h);
// 	printString("\r\n");
}