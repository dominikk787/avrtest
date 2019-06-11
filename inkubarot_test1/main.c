/*
 * inkubarot_test1.c
 *
 * Created: 2019-06-09 15:09:15
 * Author : programowanie
 */

#include <avr/io.h>
#include <usart.h>
#include <stdlib.h>
#include <scheduler.h>
#include "dht.h"
#include "1wire.h"
#include "pid.h"

#define getdigs2(d) ((d/10) % 10), (d % 10)
#define getdigs3(d) ((d/100) % 10), ((d/10) % 10), (d % 10)
#define getdigs4(d) ((d/1000) % 10), ((d/100) % 10), ((d/10) % 10), (d % 10)
#define getdigs5(d) ((d/10000) % 10), ((d/1000) % 10), ((d/100) % 10), ((d/10) % 10), (d % 10)

struct PID_DATA pidData;
Scheduler s;
uint8_t readthn;
uint8_t pidn;
uint8_t printn;

void readthf(void);
void pidf(void);
void print(void);

uint16_t h;
uint16_t t;
int16_t pid;
uint8_t pidm;

int main(void)
{
	initSch(&s);
	readthn = registerFSch(&s, readthf);
	pidn = registerFSch(&s, pidf);
	printn = registerFSch(&s, print);
	connSem(&s, readthn, readthn, SEMAPHORE_NEGATIVE);
	connSem(&s, pidn, readthn, SEMAPHORE_POSITIVE);
	connSem(&s, printn, pidn, SEMAPHORE_POSITIVE);
	pid_Init(128, 64 , 64 , &pidData);
	initUSART();
    printString("dht22 ds18b20 pid test\r\n");
	run(&s);
    while (1);
}

void readthf(void) {
	static uint8_t c = 0;
	ds18b20StartTemp();
	dht_gethumidity(&h);
	if(c && ds18b20IsReady()) {
		t = ds18b20GetTemp();
		setSem(&s, readthn, SEMAPHORE_POSITIVE);
	} else c = 1;
}

void pidf(void) {
	pid = pid_Controller(448, t, &pidData);
	pidm = pid < 0;
	pid = abs(pid);
	setSem(&s, pidn, SEMAPHORE_POSITIVE);
}

void print(void) {
	uint8_t td = (uint8_t)(((t & 0x0f) * 625) / 100);
	uint16_t ti = (t >> 4) & 0x0FFF;
	printFormat("%%.%%C %%.%&% $%%%%%\r\n",getdigs2(ti),getdigs2(td), getdigs3(h), pidm ? '-': ' ',getdigs5(pid));
	setSem(&s, readthn, SEMAPHORE_NEGATIVE);
	setSem(&s, pidn, SEMAPHORE_NEGATIVE);
}