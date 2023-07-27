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

sm_c state_machine;
static void sm_process_state();
static void sm_trigger_delay();

void setup(){
	serial_init();	
	led_init();
	ina219_init();
	
	state_machine.state = STOP;
	state_machine.prev_state = STOP;
	state_machine.delay = DELAY_600US;
}

int main(void){
	setup();
	while (1){
		sm_process_state();
		sm_trigger_delay();
	}
}

void sm_process_state(){
	switch(state_machine.state){
		case(START):
			ina219_get_voltage();
			ina219_get_power();
			ina219_get_current();
			ina219_get_shunt_voltage();
			
			sprintf(main_print_buff, "c%06d\0", shunt_sensor.current); // python script expects 6 bytes!
			serial_string(main_print_buff);
			sprintf(main_print_buff, "v%06d\0", shunt_sensor.voltage);
			serial_string(main_print_buff);
			break;
		case(STOP):
			sprintf(main_print_buff, "c%06d\0", 0);
			serial_string(main_print_buff);
			sprintf(main_print_buff, "v%06d\0", 0);
			serial_string(main_print_buff);
			break;
		case(PAUSE):
			break;
		case(CONFIG_1):
			ina219_config(INA219_CONFIG_SADCRES_12BIT_1S_532US);
			state_machine.delay = DELAY_600US;
			state_machine.state = state_machine.prev_state;
			break;
		case(CONFIG_2):
			ina219_config(INA219_CONFIG_SADCRES_12BIT_4S_2130US);
			state_machine.delay = DELAY_3MS;
			state_machine.state = state_machine.prev_state;
			break;
		case(CONFIG_3):
			ina219_config(INA219_CONFIG_SADCRES_12BIT_16S_8510US);
			state_machine.delay = DELAY_9MS;
			state_machine.state = state_machine.prev_state;
			break;
		case(CONFIG_4):
			ina219_config(INA219_CONFIG_SADCRES_12BIT_32S_17MS);
			state_machine.delay = DELAY_18MS;
			state_machine.state = state_machine.prev_state;
			break;
		case(CONFIG_5):
			ina219_change_calibration_value(CALIBRATION_40mV_400mA);
			ina219_calibrate();
			state_machine.state = state_machine.prev_state;
			break;
		case(CONFIG_6):
			ina219_change_calibration_value(CALIBRATION_80mV_800mA);
			ina219_calibrate();
			state_machine.state = state_machine.prev_state;
			break;
		case(CONFIG_7):
			ina219_change_calibration_value(CALIBRATION_160mV_1A6);
			ina219_calibrate();
			state_machine.state = state_machine.prev_state;
			break;
		case(CONFIG_8):
			ina219_change_calibration_value(CALIBRATION_320mV_2A);
			ina219_calibrate();
			state_machine.state = state_machine.prev_state;
			break;
		default: break;
	}
}

void sm_trigger_delay(){
	switch(state_machine.delay){
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
		default: _delay_us(600); break;
	}
}