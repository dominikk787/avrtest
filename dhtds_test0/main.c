/*
 * dhtds_test0.c
 *
 * Created: 2019-06-03 15:44:33
 * Author : programowanie
 */ 

#include <avr/io.h>
#include <util/delay.h>
#include "FreeRTOS.h"
#include "portable.h"
#include "startboot.h"
#include "dht11.h"
#include "USART.h"
#include "1wire.h"

#define sei asm("sei")
#define cli asm("cli")

float t;
uint8_t h;

uint8_t th_state = 0;

void boot(void);
void readthloop(void* pvParameters);
void serialloop(void* pvParameters);
uint8_t readth(void);

TaskHandle_t readthhandle;
TaskHandle_t serialhandle;

int main(void)
{
	xTaskCreate(readthloop, "readTH", 32, NULL, 2, &readthhandle);
	xTaskCreate(serialloop, "serial", 32, NULL, 2, &serialhandle);
	vTaskSuspend(serialhandle);
    boot();
	vTaskStartScheduler();
    while (1) ;
}

void boot(void) {
	initUSART();
	initUSARTstd();
	printString("dht11 ds18b20 test \r\n");
}

void readthloop(void* pvParameters) {
	transmitByte('a');
	while(1) {
		if(readth() == 2) {
			transmitByte('r');
			th_state = 1;
		}
		transmitByte('b');
		vTaskResume(serialhandle);
		vTaskSuspend(NULL);
	}
}


void serialloop(void* pvParameters) {
	transmitByte('c');
	while(1) {
		transmitByte('d');
		if(th_state){
			th_state = 0;
			float tb = t;
			uint8_t hb = h;
			printf("%ld %d\r\n", (uint32_t)(tb * 1000), hb);
		}
		vTaskResume(readthhandle)
	}
}

uint8_t readth(void) {
	static uint8_t c = 0;
	if(c == 0) {
		cli;
		ds18b20StartTemp();
		sei;
		c++;
		return 0;
	} else {
		cli;
		if(!ds18b20IsReady()) {
			sei;
			return 1;
		}
		t = ds18b20GetTemp();
		dht_gethumidity(&h);
		sei;
		c = 0;
		return 2;
	}
}