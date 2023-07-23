#ifndef __INA219_H
#define __INA219_H

#include "INA219_defines.h"

#define INA219_ADDR_MAIN		0x40

typedef enum{
	OK,
	READ_FAIL,
	WRITE_FAIL,
	FNC_FAIL,
	INIT_FAIL
}ina219_ret_t;

typedef struct _INA219_C{
	ina219_ret_t status;
	// -- configuration
	uint16_t conf_calibration_value;
	uint8_t conf_current_coef;
	uint8_t conf_power_coef;
	// -- data
	uint16_t voltage;
	uint16_t shunt_voltage;
	uint16_t current;
	uint16_t power;
}_INA219_C;

extern _INA219_C shunt_sensor;

ina219_ret_t ina219_init();
ina219_ret_t ina219_get_power();
ina219_ret_t ina219_get_current();
ina219_ret_t ina219_get_voltage();
ina219_ret_t ina219_get_shunt_voltage();

#endif