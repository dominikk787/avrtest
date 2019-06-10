/*
 * inkubarot_test1.c
 *
 * Created: 2019-06-09 15:09:15
 * Author : programowanie
 */

#include <avr/io.h>
#include <usart.h>
#include "dht.h"
#include "1wire.h"
#include "pid.h"

#define getdigs2(d) ((d/10) % 10), (d % 10)
#define getdigs3(d) ((d/100) % 10), ((d/10) % 10), (d % 10)
#define getdigs4(d) ((d/1000) % 10), ((d/100) % 10), ((d/10) % 10), (d % 10)
#define getdigs5(d) ((d/10000) % 10), ((d/1000) % 10), ((d/100) % 10), ((d/10) % 10), (d % 10)

struct PID_DATA pidData;

int main(void)
{
	pid_Init(128, 64 , 64 , &pidData);
	initUSART();
    printString("dht22 ds18b20 pid test\r\n");
    while (1)
    {
		uint16_t t = ds18b20ReadTemp();
		uint8_t td = (uint8_t)(((t & 0x0f) * 625) / 100);
		uint16_t ti = (t >> 4) & 0x0FFF;
		uint16_t h;
		dht_gethumidity(&h);
		uint16_t pid = pid_Controller(448, t, &pidData) + 0x8000;
		printFormat("%%.%% %%.% %%%%%\r\n",getdigs2(ti),getdigs2(td), getdigs3(h),getdigs5(pid));
    }
}

