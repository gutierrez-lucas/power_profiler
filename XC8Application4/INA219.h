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
	ina219_calibration_t conf_calibration_value;
	ina219_gain_t conf_gain_value;
	// -- data
	int16_t voltage;
	int16_t shunt_voltage;
	int16_t current;
	int16_t power;
}_INA219_C;

extern _INA219_C shunt_sensor;

ina219_ret_t ina219_init();
ina219_ret_t ina219_get_power();
ina219_ret_t ina219_get_current();
ina219_ret_t ina219_get_voltage();
ina219_ret_t ina219_get_shunt_voltage();
ina219_ret_t ina219_config(ina219_rate_t rate);
void ina219_change_calibration_value(ina219_calibration_t new_calib);
ina219_ret_t ina219_calibrate();
#endif