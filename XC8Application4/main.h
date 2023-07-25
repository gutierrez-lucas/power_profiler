#ifndef __MAIN
#define __MAIN

#define F_CPU 16000000UL	/* Define CPU frequency here 8MHz */

#define PRINT_SENSOR_DATA
#ifdef PRINT_SENSOR_DATA
#undef PRINT_SYSTEM_INFO
char main_print_buff[50];
#endif

typedef enum{
	START,
	STOP,
	PAUSE,
	CONFIG_1,
	CONFIG_2,
	CONFIG_3,
	CONFIG_4,
	FAIL
}sm_command_t;

extern sm_command_t sm;
extern sm_command_t sm_prev;

#endif
