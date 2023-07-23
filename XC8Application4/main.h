#ifndef __MAIN
#define __MAIN

#define F_CPU 16000000UL	/* Define CPU frequency here 8MHz */

typedef enum{
	START,
	STOP,
	PAUSE,
	CONFIG,
	FAIL
}sm_command_t;

extern sm_command_t sm;

#endif
