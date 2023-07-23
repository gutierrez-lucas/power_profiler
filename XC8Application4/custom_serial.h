#ifndef __C_SERIAL
#define __C_SERIAL
#include "main.h"
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#define USART_BAUDRATE  9600
#define UBRR_VALUE      (((F_CPU/(USART_BAUDRATE*16UL)))-1)
#define RX_BUFFER_SIZE  512
#define RX_LINE_SIZE    128

char rx_buffer[RX_BUFFER_SIZE], rx_line[RX_LINE_SIZE];
char rx_buffer_pos, rx_line_pos;
uint8_t waiting;

void serial_init();
void serial_char(char data);
void serial_break();
void serial_string(const char* s);

bool compare_string(char *first, const char *second);
sm_command_t logic_handler();
#endif