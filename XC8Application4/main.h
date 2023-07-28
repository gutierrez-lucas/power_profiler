#ifndef __MAIN
#define __MAIN

#define F_CPU 16000000UL	// 16MHz clock 

//#define PRINT_SYSTEM_INFO  // dev debug mode, not external usage

#define PRINT_SENSOR_DATA	//external usage mode
#ifdef PRINT_SENSOR_DATA	
#undef PRINT_SYSTEM_INFO	// mutually exclusive with the plotter. 
char main_print_buff[50];
#endif

#endif
