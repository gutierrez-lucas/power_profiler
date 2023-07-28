#ifndef __C_SERIAL
#define __C_SERIAL

#include "main.h"
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#define RX_BUFFER_SIZE		512
#define RX_LINE_SIZE		128
char  rx_line[RX_LINE_SIZE];
uint16_t  rx_line_pos;

void serial_init();
void serial_char(char data);
void serial_string(const char* s);

#endif