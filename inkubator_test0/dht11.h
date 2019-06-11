/*
 * dht11.h
 *
 * Created: 2019-06-03 17:22:53
 *  Author: programowanie
 */ 


#ifndef DHT11_H_
#define DHT11_H_

#include <stdio.h>
#include <avr/io.h>

//setup port
#define DHT_DDR DDRB
#define DHT_PORT PORTB
#define DHT_PIN PINB
#define DHT_INPUTPIN PORTB1

//sensor type
#define DHT_DHT11 1
#define DHT_DHT22 2
#define DHT_TYPE DHT_DHT22

//enable decimal precision (float)
#if DHT_TYPE == DHT_DHT11
#define DHT_FLOAT 0
#elif DHT_TYPE == DHT_DHT22
#define DHT_FLOAT 1
#endif

//timeout retries
#define DHT_TIMEOUT 200

//functions
#if DHT_FLOAT == 1
extern int8_t dht_gettemperature(int16_t *temperature);
extern int8_t dht_gethumidity(uint16_t *humidity);
extern int8_t dht_gettemperaturehumidity(int16_t *temperature, uint16_t *humidity);
#elif DHT_FLOAT == 0
extern int8_t dht_gettemperature(uint8_t *temperature);
extern int8_t dht_gethumidity(uint8_t *humidity);
extern int8_t dht_gettemperaturehumidity(uint8_t *temperature, uint8_t *humidity);
#endif

#endif /* DHT11_H_ */