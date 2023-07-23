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
			case(CONFIG):
				break;
			default: break;
		}
		_delay_ms(1000);
	}
}