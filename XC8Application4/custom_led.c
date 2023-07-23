#include <avr/io.h>
#include "custom_led.h"

void led_init(){ DDRB |= BUILD_IN_LED; }
void led_turn_on(){ PORTB |= BUILD_IN_LED; }
void led_turn_off(){ PORTB &= ~BUILD_IN_LED; }
void led_toogle(){ PORTB ^= BUILD_IN_LED; }