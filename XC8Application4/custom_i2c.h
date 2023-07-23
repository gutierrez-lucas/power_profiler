#ifndef TWI_MASTER_H_
#define TWI_MASTER_H_

#include <avr/io.h>
#include <util/twi.h>
#include <stdbool.h>

#define DEBUG_LOG			0
#define SUCCESS				0

#define DONT_REDO_START		false
#define REDO_START			true

#define TW_SCL_PIN			PORTC5
#define TW_SDA_PIN			PORTC4

#define TW_SLA_W(ADDR)		((ADDR << 1) | TW_WRITE)
#define TW_SLA_R(ADDR)		((ADDR << 1) | TW_READ)
#define TW_READ_ACK			1
#define TW_READ_NACK		0

typedef uint16_t ret_code_t;

enum{
	LITTLE_ENDIAN,
	BIG_ENDIAN
};

void ERROR_CHECK(ret_code_t error_code);

typedef enum {
	TW_FREQ_100K,
	TW_FREQ_250K,
	TW_FREQ_400K
} i2c_freq;

void i2c_init(i2c_freq freq, bool pullup_en);
ret_code_t i2c_write(uint8_t slave_addr, uint8_t* p_data, uint8_t len, bool repeat_start, bool endianness);
ret_code_t i2c_read(uint8_t slave_addr, uint8_t* p_data, uint8_t len, bool endianness);

#endif /* TWI_MASTER_H_ */