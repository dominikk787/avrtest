/*
 * scheduler_test1.c
 *
 * Created: 2019-06-06 17:51:08
 * Author : programowanie
 */ 

#include <avr/io.h>
#include "scheduler.h"
#include "1wire.h"
#include "dht11.h"
#include "USART.h"
#include "lcd.h"
#include "avr/pgmspace.h"

const uint8_t tempchar[] PROGMEM = {0b00110000,0b01001111,0b01001111,0b00110000};
const uint8_t humchar[] PROGMEM = {0b00111000,0b01001100,0b01010100,0b01000100,0b00111000};
const uint8_t degchar[] PROGMEM = {0b00000110,0b00001001,0b00001001,0b00000110};

uint16_t t = 0;
uint16_t h = 0;

void readt(void);
void readh(void);
void print(void);
void lcd(void);

Scheduler sch;
uint8_t readtn;
uint8_t readhn;
uint8_t printn;
uint8_t lcdn;

int main(void)
{
	initSch(&sch);
	readtn = registerFSch(&sch,readt);
	readhn = registerFSch(&sch,readh);
	printn = registerFSch(&sch,print);
	lcdn = registerFSch(&sch,lcd);
	connSem(&sch, readtn, readtn, SEMAPHORE_NEGATIVE);
	connSem(&sch, readhn, readhn, SEMAPHORE_NEGATIVE);
	connSem(&sch, printn, readtn, SEMAPHORE_POSITIVE);
	connSem(&sch, printn, readhn, SEMAPHORE_POSITIVE);
	connSem(&sch, lcdn, printn, SEMAPHORE_POSITIVE);
	initUSART();
	lcd_init();
	lcd_clear();
	lcd_text("Scheduler test",0,0);
	lcd_text("n5110 lcd lib",0,1);
	lcd_text("Read temp hum",0,2);
	lcd_refresh();
	printString("scheduler test\r\n");
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

void print(void) {
	printFormat("%%.%%C %%.%&%\r\n", (t / 1000) % 10, (t / 100) % 10, (t / 10) % 10, t % 10, (h / 100) % 10, (h / 10) % 10, h % 10);
	setSem(&sch, readtn, SEMAPHORE_NEGATIVE);
	setSem(&sch, readhn, SEMAPHORE_NEGATIVE);
	setSem(&sch, printn, SEMAPHORE_POSITIVE);
}

void lcd(void) {
	char tb[7];
	char hb[4];
	processFormat(tb, "%%.%%", (t / 1000) % 10, (t / 100) % 10, (t / 10) % 10, t % 10);
	processFormat(hb, "%%.%&%", (h / 100) % 10, (h / 10) % 10, h % 10);
	lcd_clear();
	lcd_image(tempchar,0,0,4,1);
	lcd_text(tb,6,0);
	lcd_image(degchar,36,0,4,1);
	lcd_text("C",41,0);
	lcd_image(humchar,0,1,5,1);
	lcd_text(hb,6,1);
	lcd_refresh();
	setSem(&sch, printn, SEMAPHORE_NEGATIVE);
}