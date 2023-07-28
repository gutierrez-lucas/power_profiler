#ifndef __MAIN
#define __MAIN

#define F_CPU 16000000UL	// 16MHz clock 

#define PRINT_SENSOR_DATA
#ifdef PRINT_SENSOR_DATA	// this is for python plotter usage
#undef PRINT_SYSTEM_INFO	// this is for dev debug info, mutually exclusive with the plotter. 
char main_print_buff[50];
#endif

#endif
