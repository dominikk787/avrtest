/*
 * inkubator_test0.c
 *
 * Created: 2019-06-09 15:09:15
 * Author : programowanie
 */ 

#include <avr/io.h>
#include <scheduler.h>
#include <n5110_lcd.h>
#include <usart.h>
#include "1wire.h"
#include "dht11.h"
#include "pid.h"
#include <avr/pgmspace.h>

#define getdig(d, zn) ((d / tenpots[zn]) % 10)

const uint16_t tenpots[5] = {1,10,100,1000,10000};

const uint8_t tempchar[] PROGMEM = {0b00110000,0b01001111,0b01001111,0b00110000};
const uint8_t humchar[] PROGMEM = {0b00111000,0b01001100,0b01010100,0b01000100,0b00111000};
const uint8_t degchar[] PROGMEM = {0b00000110,0b00001001,0b00001001,0b00000110};

uint16_t t = 0;
uint16_t h = 0;
uint16_t pidv = 0;

void readt(void);
void readh(void);
void pidcalc(void);
void lcd(void);

Scheduler sch;
uint8_t readtn;
uint8_t readhn;
uint8_t pidcalcn;
uint8_t lcdn;

int main(void)
{
	initSch(&sch);
	readtn = registerFSch(&sch,readt);
	readhn = registerFSch(&sch,readh);
	pidcalcn = registerFSch(&sch,pidcalc);
	lcdn = registerFSch(&sch,lcd);
	connSem(&sch, readtn, readtn, SEMAPHORE_NEGATIVE);
	connSem(&sch, readhn, readhn, SEMAPHORE_NEGATIVE);
	connSem(&sch, pidcalcn, readtn, SEMAPHORE_POSITIVE);
	connSem(&sch, pidcalcn, readhn, SEMAPHORE_POSITIVE);
	connSem(&sch, lcdn, pidcalcn, SEMAPHORE_POSITIVE);
	lcd_init();
	lcd_clear();
	lcd_text("Scheduler test",0,0);
	lcd_text("n5110 lcd lib",0,1);
	lcd_text("Read temp hum",0,2);
	lcd_refresh();
	run(&sch);
	while (1) ;
}

void readt(void) {
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
	setSem(&sch, readtn, SEMAPHORE_POSITIVE);
}

void readh(void) {
	dht_gethumidity(&h);
	setSem(&sch, readhn, SEMAPHORE_POSITIVE);
}

void pidcalc(void) {
	pidv = pid(t,2800);
	setSem(&sch, readtn, SEMAPHORE_NEGATIVE);
	setSem(&sch, readhn, SEMAPHORE_NEGATIVE);
	setSem(&sch, pidcalcn, SEMAPHORE_POSITIVE);
}

void lcd(void) {
	char tb[7];
	char hb[4];
	char pidbuf[6];
	processFormat(tb, "%%.%%", getdig(t,3), getdig(t,2), getdig(t,1), getdig(t,0));
	processFormat(hb, "%%.%&%", getdig(h,2), getdig(h,1), getdig(h,0));
	processFormat(pidbuf, "%%%%%", getdig(pidv,4), getdig(pidv,3), getdig(pidv,2), getdig(pidv,1), getdig(pidv,0));
	lcd_clear();
	lcd_image(tempchar,0,0,4,1);
	lcd_text(tb,6,0);
	lcd_image(degchar,36,0,4,1);
	lcd_text("C",41,0);
	lcd_image(humchar,0,1,5,1);
	lcd_text(hb,6,1);
	lcd_text("pid: ",0,4);
	lcd_text(pidbuf,30,4);
	lcd_refresh();
	setSem(&sch, pidcalcn, SEMAPHORE_NEGATIVE);
}

