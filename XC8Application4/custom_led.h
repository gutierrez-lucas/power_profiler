#ifndef __C_LED
#define __C_LED

#define BUILD_IN_LED 0b00100000

void led_init();
void led_turn_on();
void led_turn_off();
void led_toogle();

#endif