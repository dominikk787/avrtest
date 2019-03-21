/*
 * i2c.c
 *
 * Created: 2019-03-11 14:46:45
 *  Author: programowanie
 */ 

#include <avr/io.h>
#include "i2c.h"

void i2cInit(void) {
	TWCR = _BV(TWEN);
	TWBR = (F_CPU/F_SCL - 16) / 2;
}

uint8_t i2cStart(void) {
	TWCR = _BV(TWEN) | _BV(TWSTA) | _BV(TWINT);
	while (!(TWCR & _BV(TWINT)));
	uint8_t status = TWSR & I2C_STATUS_MASK;
	if (status == I2C_START || status == I2C_START_REPEATED) {
		return I2C_OK;
		} else {
		return status;
	}
}

void i2cStop(void) {
	TWCR = _BV(TWEN) | _BV(TWSTO) | _BV(TWINT);
	while (TWCR & _BV(TWSTO));
}

void i2cSend(uint8_t data) {
	TWDR = data;
	TWCR = _BV(TWEN) | _BV(TWINT);
	while (!(TWCR & _BV(TWINT)));
}

uint8_t i2cSendAddress(uint8_t address) {
	i2cSend(address);
	uint8_t status = TWSR & I2C_STATUS_MASK;
	if (address & 0b00000001) {
		return (status == I2C_ADDR_READ_ACK) ? I2C_OK : status;
		} else {
		return (status == I2C_ADDR_WRITE_ACK) ? I2C_OK : status;
	}
}

uint8_t i2cSendData(uint8_t data) {
	i2cSend(data);
	uint8_t status = TWSR & I2C_STATUS_MASK;
	return (status == I2C_DATA_SENT_ACK) ? I2C_OK : status;
}

uint8_t i2cReceive(uint8_t last, uint8_t * data){
	TWCR = last ? _BV(TWEN) | _BV(TWINT) : _BV(TWEN) | _BV(TWINT) | _BV(TWEA);
	while (!(TWCR & (1<<TWINT)));
	*data = TWDR;
	uint8_t status = TWSR & I2C_STATUS_MASK;
	if (last) {
		return (status == I2C_DATA_RECEIVED_NACK) ? I2C_OK : status;
		} else {
		return (status == I2C_DATA_RECEIVED_ACK) ? I2C_OK : status;
	}
}