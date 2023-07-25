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

sm_command_t sm = STOP;
sm_command_t sm_prev = STOP;

void setup(){
	serial_init();	
	led_init();
	ina219_init();
}

typedef enum{
	DELAY_600US = 1,
	DELAY_3MS,
	DELAY_9MS,
	DELAY_18MS
}sm_delay_t;

int main(void){
	sm_delay_t rate_delay_mode = DELAY_600US; 
	setup();
	while (1){
		switch(sm){
			case(START):
				ina219_get_voltage();
				ina219_get_power();
				ina219_get_current();
				ina219_get_shunt_voltage();
#ifdef PRINT_SENSOR_DATA
				sprintf(main_print_buff, "%06d", shunt_sensor.current); // python script expects 6 bytes!
				serial_string(main_print_buff);
#endif				
				break;
			case(STOP):
				sprintf(main_print_buff, "%06d", 0); // python script expects 6 bytes!
				serial_string(main_print_buff);
				break;
			case(PAUSE):
				break;	
			case(CONFIG_1):
				ina219_config(INA219_CONFIG_SADCRES_12BIT_1S_532US);	
				rate_delay_mode = DELAY_600US;			
				sm = sm_prev;
				break;
			case(CONFIG_2):
				ina219_config(INA219_CONFIG_SADCRES_12BIT_4S_2130US);
				rate_delay_mode = DELAY_3MS;
				sm = sm_prev;
				break;
			case(CONFIG_3):
				ina219_config(INA219_CONFIG_SADCRES_12BIT_16S_8510US);
				rate_delay_mode = DELAY_9MS;
				sm = sm_prev;
				break;
			case(CONFIG_4):
				ina219_config(INA219_CONFIG_SADCRES_12BIT_32S_17MS);
				rate_delay_mode = DELAY_18MS;
				sm = sm_prev;
				break;
			default: break;
		}
		switch(rate_delay_mode){
			case(DELAY_600US):
				_delay_us(600);
				break;
			case(DELAY_3MS):
				_delay_ms(3);
				break;
			case(DELAY_9MS):
				_delay_ms(9);
				break;
			case(DELAY_18MS):
				_delay_ms(18);
				break;
			defautl: _delay_us(600); break;
		}
	}
}