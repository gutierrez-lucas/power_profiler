#include "main.h"
#include <stdio.h>
#include <util/delay.h>
#include "custom_serial.h"
#include "fms.h"
#include "INA219.h"

sm_c state_machine;

void sm_init(){
	state_machine.state = STOP;
	state_machine.prev_state = STOP;
	state_machine.delay = DELAY_600US;
}

void sm_process_state(){
	switch(state_machine.state){
		case(START):
			ina219_get_voltage();
			ina219_get_power();
			ina219_get_current();
			ina219_get_shunt_voltage();
		
			// the extra \0 is a needed reference for the python plotter 
			sprintf(main_print_buff, "c%06d\0", shunt_sensor.current); // python script expects 7 bytes!
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
			break; // just do nothing
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

sm_command_t sm_parser() {
	if(!strncmp(rx_line, "start", strlen(rx_line))) {
#ifdef PRINT_SYSTEM_INFO		
			serial_string("START\n");
#endif			
			return(START);
		}else if(!strncmp(rx_line, "stop", strlen(rx_line))) {
#ifdef PRINT_SYSTEM_INFO
			serial_string("STOP\n");
#endif
			return(STOP);
		}else if(!strncmp(rx_line, "pause", strlen(rx_line))) {
#ifdef PRINT_SYSTEM_INFO
			serial_string("PAUSE\n");
#endif
			return(PAUSE);		
		}else if(!strncmp(rx_line, "config1", strlen(rx_line))) {
#ifdef PRINT_SYSTEM_INFO
			serial_string("CONFIG_1\n");
#endif
			return(CONFIG_1);		
		}else if(!strncmp(rx_line, "config2", strlen(rx_line))) {
#ifdef PRINT_SYSTEM_INFO
			serial_string("CONFIG_2\n");
#endif
			return(CONFIG_2);
		}else if(!strncmp(rx_line, "config3", strlen(rx_line))) {
#ifdef PRINT_SYSTEM_INFO
			serial_string("CONFIG_3\n");
#endif
			return(CONFIG_3);
		}else if(!strncmp(rx_line, "config4", strlen(rx_line))) {
#ifdef PRINT_SYSTEM_INFO
			serial_string("CONFIG_4\n");
#endif
			return(CONFIG_4);
		}else if(!strncmp(rx_line, "config5", strlen(rx_line))) {
#ifdef PRINT_SYSTEM_INFO
			serial_string("CONFIG_5\n");
#endif
			return(CONFIG_5);						
		}else if(!strncmp(rx_line, "config6", strlen(rx_line))) {
#ifdef PRINT_SYSTEM_INFO
			serial_string("CONFIG_6\n");
#endif
			return(CONFIG_6);
		}else if(!strncmp(rx_line, "config7", strlen(rx_line))) {
#ifdef PRINT_SYSTEM_INFO
			serial_string("CONFIG_7\n");
#endif
			return(CONFIG_7);			
		}else if(!strncmp(rx_line, "config8", strlen(rx_line))) {
#ifdef PRINT_SYSTEM_INFO
			serial_string("CONFIG_8\n");
#endif
			return(CONFIG_8);
		}else{
#ifdef PRINT_SYSTEM_INFO
			serial_string("FAIL\n");
			// Preview current
			serial_string("input: ");
			serial_string(rx_line);
			serial_string("\n");
			// Handle overflow
			if(rx_buffer_pos >= RX_BUFFER_SIZE){
				rx_buffer_pos = 0;
				memset(rx_buffer, 0, RX_BUFFER_SIZE);
			}
#endif
		return(FAIL);
	}
}