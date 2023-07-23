#include <avr/io.h>
#include <avr/interrupt.h>
#include <string.h>
#include "custom_serial.h"
#include "main.h"

ISR(USART_RX_vect)
{
	if(waiting == 0) // Empty buffer
	memset(rx_line, 0, RX_LINE_SIZE);

	waiting = 1; // Block

	unsigned char input = UDR0; 	// Receive data
	if(input != '\n')
	rx_line[rx_line_pos++] = input;

	if(rx_line_pos >= RX_LINE_SIZE || (input == '\n' && rx_line_pos > 0)) { 	// Handle interrupt
		sm = logic_handler(); 		// Handle result
		rx_line_pos = 0;  // Resize
		waiting = 0; // Unblock
	}
}

void serial_init(){
	UBRR0 = UBRR_VALUE;			// set baud rate
	UCSR0B|=(1<<TXEN0);			// enable TX
	UCSR0B|=(1<<RXEN0);			// enable RX
	UCSR0B|=(1<<RXCIE0);			// RX complete interrupt
	UCSR0C|=(1<<UCSZ01)|(1<<UCSZ01); 	// no parity, 1 stop bit, 8-bit data
	
	sei();
	
	rx_buffer_pos = 0;
	rx_line_pos = 0;
	waiting = 0;
}

void serial_char(char data){
	while(!(UCSR0A & (1<<UDRE0)));
	UDR0 = data;
}

void serial_string(const char* s) {
	while (*s != '\0')
	serial_char(*s++);
}

bool compare_string(char *first, const char *second) {
	while (*first == *second && *first != '\0') {
		first++;
		second++;
	}
	// 0 -> not equal
	// 1 -> equal
	return *first == '\0' && *second == '\0';
}

sm_command_t logic_handler() {
	char print_h[50];
	// Check commands
	sprintf(print_h, "%s - %d\n", rx_line, strlen(rx_line));
	serial_string(print_h);
	if(!strncmp(rx_line, "start", strlen(rx_line))) {
			serial_string("START\n");
			return(START);
		}else if(!strncmp(rx_line, "/stop", strlen(rx_line))) {
			serial_string("STOP\n");
			return(STOP);
		}else if(!strncmp(rx_line, "/pause", strlen(rx_line))) {
			serial_string("PAUSE\n");
			return(PAUSE);
		}else if(!strncmp(rx_line, "/config", strlen(rx_line))) {
			serial_string("CONFIG\n");
			return(CONFIG);
		}else{
			serial_string("FAIL\n");
			// Preview current
			serial_string("input: ");
			serial_string(rx_line);
			serial_string("\n");
			// Handle overflow
			if(rx_buffer_pos >= RX_BUFFER_SIZE){
				rx_buffer_pos = 0;
				memset(rx_buffer, 0, RX_BUFFER_SIZE);
			}
		return(FAIL);
	}
}