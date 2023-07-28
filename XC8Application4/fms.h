#ifndef __FMS
#define __FMS

typedef enum{
	START,
	STOP,
	PAUSE,
	CONFIG_1,
	CONFIG_2,
	CONFIG_3,
	CONFIG_4,
	CONFIG_5,
	CONFIG_6,
	CONFIG_7,
	CONFIG_8,
	FAIL
}sm_command_t;

typedef enum{
	DELAY_600US = 1,
	DELAY_3MS,
	DELAY_9MS,
	DELAY_18MS
}sm_delay_t;

typedef struct sm{
	sm_command_t state;
	sm_command_t prev_state;
	sm_delay_t	delay;
}sm_c;

void sm_init();
void sm_process_state();
void sm_trigger_delay();
sm_command_t sm_parser();

extern sm_c state_machine;
extern sm_command_t sm;
extern sm_command_t sm_prev;

#endif