#define F_CPU 16000000UL

#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "main.h"
#include "custom_serial.h"
#include "custom_led.h"
#include "INA219.h"

#define PRINT_SENSOR_DATA
#ifdef PRINT_SENSOR_DATA
char main_print_buff[50];
#endif

sm_command_t sm = STOP;

void setup(){
	serial_init();	
	led_init();
	ina219_init();
}

int main(void){

	setup();
	uint8_t counter = 0;
	while (1){
		switch(sm){
			case(START):
				ina219_get_voltage();
				ina219_get_power();
				ina219_get_current();
				ina219_get_shunt_voltage();
#ifdef PRINT_SENSOR_DATA
				sprintf(main_print_buff, "V:%d|SV:%d|C:%d|P:%d\n", shunt_sensor.voltage, shunt_sensor.shunt_voltage, shunt_sensor.current, shunt_sensor.power);
				serial_string(main_print_buff);
#endif				
				break;
			case(STOP):
				break;
			case(PAUSE):
				break;	
			case(CONFIG_1):
				ina219_change_calibration_value(CALIBRATION_40mV_400mA);
				ina219_calibrate();
				ina219_config(INA219_CONFIG_GAIN_1_40mV_400mA);
				break;
			case(CONFIG_2):
				ina219_change_calibration_value(CALIBRATION_80mV_800mA);
				ina219_calibrate();
				ina219_config(INA219_CONFIG_GAIN_2_80mV_800mA);
				break;
			case(CONFIG_3):
				ina219_change_calibration_value(CALIBRATION_160mV_1A6);
				ina219_calibrate();
				ina219_config(INA219_CONFIG_GAIN_4_160mV_1A6);
				break;
			case(CONFIG_4):
				ina219_change_calibration_value(CALIBRATION_320mV_2A);
				ina219_calibrate();
				ina219_config(INA219_CONFIG_GAIN_8_320mV_2A);
				break;
			default: break;
		}
		_delay_ms(200);
	}
}