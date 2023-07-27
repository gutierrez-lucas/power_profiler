#include <avr/io.h>
#include <stdio.h>
#include "INA219.h"
#include "custom_i2c.h"
#include "custom_serial.h"

//#define INA219_DEBUG
#ifdef INA219_DEBUG
static char print_buffer[50];
#endif

#define BUFFER_IN		(uint8_t*)&data_in.sb

_INA219_C shunt_sensor;

union my_byte{   
// this is a tool for automatic conversion between full byte -> low byte + high byte
	struct sp{
		uint8_t lb; // little endian
		uint8_t hb;
	}sb;
	uint16_t fb;
}data_in;

ina219_ret_t ina219_calibrate(){
	uint8_t data_out[3];
	data_in.fb = shunt_sensor.conf_calibration_value;
	data_out[0] = data_in.sb.lb;
	data_out[1] = data_in.sb.hb;
	data_out[2] = CALIBRATION_REGISTER;
#ifdef INA219_DEBUG	
	sprintf(print_buffer, "---\nCALIB: 0x%X - 0x%X - 0x%X\n", data_out[0], data_out[1], data_out[2]);
	serial_string(print_buffer);
#endif	
	
	if( i2c_write(INA219_ADDR_MAIN, data_out, 3, DONT_REDO_START, LITTLE_ENDIAN) != SUCCESS ){
		serial_string("Write device failed\n");
		return(WRITE_FAIL);
	}
	return(OK);
}

ina219_ret_t ina219_config(ina219_rate_t rate){ // by the moment I only care about gain configuration by the end user
	uint8_t data_out[3];
	
	uint16_t config =	INA219_CONFIG_BVOLTAGERANGE_16V |
						INA219_CONFIG_GAIN_1_40mV_400mA |
						INA219_CONFIG_BADCRES_12BIT |
						rate |
						INA219_CONFIG_MODE_SANDBVOLT_CONTINUOUS;
	
	data_in.fb = config;
	data_out[0] = data_in.sb.lb;
	data_out[1] = data_in.sb.hb;
	data_out[2] = CONFIGURATION_REGISTER;
#ifdef INA219_DEBUG	
	sprintf(print_buffer, "---\nConfig: 0x%X - 0x%X - 0x%X\n", data_out[0], data_out[1], data_out[2]);
	serial_string(print_buffer);
#endif	
	
	if( i2c_write(INA219_ADDR_MAIN, data_out, 3, DONT_REDO_START, LITTLE_ENDIAN) != SUCCESS ){
		serial_string("Write device failed\n");
		return(WRITE_FAIL);
	}
	return(OK);
}

void ina219_change_calibration_value(ina219_calibration_t new_calib){
	shunt_sensor.conf_calibration_value = new_calib; // default
	switch(new_calib){
		case(CALIBRATION_40mV_400mA):	shunt_sensor.current_lsb_adj = 1; break;
		case(CALIBRATION_80mV_800mA):	shunt_sensor.current_lsb_adj = 2; break;
		case(CALIBRATION_160mV_1A6):	shunt_sensor.current_lsb_adj = 4; break;
		case(CALIBRATION_320mV_2A):		shunt_sensor.current_lsb_adj = 8; break;
		default:						shunt_sensor.current_lsb_adj = 1; break;
	}
}

ina219_ret_t ina219_init(){
	i2c_init(TW_FREQ_400K, true);
	ina219_change_calibration_value(CALIBRATION_40mV_400mA);
	if(ina219_calibrate() != OK) return(WRITE_FAIL);
	if(ina219_config(INA219_CONFIG_SADCRES_12BIT_1S_532US)) return(WRITE_FAIL);
	
	return(OK);
}

ina219_ret_t ina219_get_voltage(){
	data_in.sb.lb = BUS_VOLTAGE_REGISTER;
	if( i2c_write(INA219_ADDR_MAIN, &data_in.sb.lb, 1, REDO_START, LITTLE_ENDIAN) == SUCCESS ){
		if( i2c_read(INA219_ADDR_MAIN, BUFFER_IN, 2, LITTLE_ENDIAN) == SUCCESS ){
			shunt_sensor.voltage = 4*(data_in.fb>>3);
#ifdef INA219_DEBUG
			sprintf(print_buffer, "Data in: %d (%u : %d - %d)\n", shunt_sensor.voltage, data_in.fb, data_in.sb.hb, data_in.sb.lb);
			serial_string(print_buffer);
#endif
			return(OK);
		}else{
#ifdef PRINT_SYSTEM_INFO
			serial_string("Data read failed\n");
#endif
			return(READ_FAIL);
		}
	}else{
#ifdef PRINT_SYSTEM_INFO
		serial_string("Write device failed\n");
#endif
		return(WRITE_FAIL);
	}
	return(FNC_FAIL);
}

ina219_ret_t ina219_get_power(){
	ina219_calibrate();
	data_in.sb.lb = POWER_REGISTER;
	if( i2c_write(INA219_ADDR_MAIN, &data_in.sb.lb, 1, REDO_START, LITTLE_ENDIAN) == SUCCESS ){
		if( i2c_read(INA219_ADDR_MAIN, BUFFER_IN, 2, LITTLE_ENDIAN) == SUCCESS ){
			shunt_sensor.power = data_in.fb;
#ifdef INA219_DEBUG
			sprintf(print_buffer, "Data in: %d (%d : %d - %d)\n", shunt_sensor.power, data_in.fb, data_in.sb.hb, data_in.sb.lb);
			serial_string(print_buffer);
#endif
			return(OK);
		}else{
#ifdef PRINT_SYSTEM_INFO
			serial_string("Data read failed\n");
#endif
			return(READ_FAIL);
		}
	}else{
#ifdef PRINT_SYSTEM_INFO
		serial_string("Write device failed\n");
#endif
		return(WRITE_FAIL);
	}
	return(FNC_FAIL);
}


ina219_ret_t ina219_get_current(){
	ina219_calibrate();
	data_in.sb.lb = CURRENT_REGISTER;
	if( i2c_write(INA219_ADDR_MAIN, &data_in.sb.lb, 1, REDO_START, LITTLE_ENDIAN) == SUCCESS ){
		if( i2c_read(INA219_ADDR_MAIN, BUFFER_IN, 2, LITTLE_ENDIAN) == SUCCESS ){
			shunt_sensor.current = (data_in.fb)*shunt_sensor.current_lsb_adj;
#ifdef INA219_DEBUG
			sprintf(print_buffer, "Data in: %d (%d : %d - %d)\n", shunt_sensor.current, data_in.fb, data_in.sb.hb, data_in.sb.lb);
			serial_string(print_buffer);
#endif
			return(OK);
		}else{
#ifdef PRINT_SYSTEM_INFO
			serial_string("Data read failed\n");
#endif
			return(READ_FAIL);
		}
	}else{
#ifdef PRINT_SYSTEM_INFO
		serial_string("Write device failed\n");
#endif
		return(WRITE_FAIL);
	}
	return(FNC_FAIL);
}

ina219_ret_t ina219_get_shunt_voltage(){
	data_in.sb.lb = SHUNT_VOLTAGE_REGISTER;
	if( i2c_write(INA219_ADDR_MAIN, &data_in.sb.lb, 1, REDO_START, LITTLE_ENDIAN) == SUCCESS ){
		if( i2c_read(INA219_ADDR_MAIN, BUFFER_IN, 2, LITTLE_ENDIAN) == SUCCESS ){
			shunt_sensor.shunt_voltage = data_in.fb;
#ifdef INA219_DEBUG
			sprintf(print_buffer, "Data in: %d (%d : %d - %d)\n", shunt_sensor.shunt_voltage, data_in.fb, data_in.sb.hb, data_in.sb.lb);
			serial_string(print_buffer);
#endif
			return(OK);
		}else{
			return(READ_FAIL);
#ifdef PRINT_SYSTEM_INFO
			serial_string("Data read failed\n");
#endif
		}
	}else{
		return(WRITE_FAIL);
#ifdef PRINT_SYSTEM_INFO
		serial_string("Write device failed\n");
#endif		
	}
	return(FNC_FAIL);
}
