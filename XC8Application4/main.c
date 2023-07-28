#include "main.h"
#include <avr/io.h>
#include "custom_serial.h"
#include "INA219.h"
#include "fms.h"

void setup(){
	sm_init();
	serial_init();	
	ina219_init();
}

int main(void){
	setup();
	while (1){
		sm_process_state();
		sm_trigger_delay();
	}
}
