#include "custom_i2c.h"
#include "custom_serial.h"
#include <stdio.h>
#include <string.h>

//#define I2C_DEBUG
#ifdef I2C_DEBUG
static char i2c_print_buffer[50];
#endif

static ret_code_t tw_start(void)
{
	/* Send START condition */
	TWCR =  (1 << TWINT) | (1 << TWEN) | (1 << TWSTA);
	
	/* Wait for TWINT flag to set */
	while (!(TWCR & (1 << TWINT)));
	
	/* Check error */
	if (TW_STATUS != TW_START && TW_STATUS != TW_REP_START)
	{
		return TW_STATUS;
	}

	return SUCCESS;
}


static void tw_stop(void)
{
	/* Send STOP condition */

	TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWSTO);
}


static ret_code_t tw_write_sla(uint8_t sla)
{
	/* Transmit slave address with read/write flag */

	TWDR = sla;
	TWCR = (1 << TWINT) | (1 << TWEN);
	
	/* Wait for TWINT flag to set */
	while (!(TWCR & (1 << TWINT)));
	if (TW_STATUS != TW_MT_SLA_ACK && TW_STATUS != TW_MR_SLA_ACK)
	{

		return TW_STATUS;
	}


	return SUCCESS;
}


static ret_code_t tw_write(uint8_t data)
{
	/* Transmit 1 byte*/

	TWDR = data;
	TWCR = (1 << TWINT) | (1 << TWEN);
	
	/* Wait for TWINT flag to set */
	while (!(TWCR & (1 << TWINT)));
	if (TW_STATUS != TW_MT_DATA_ACK)
	{

		return TW_STATUS;
	}
	
	return SUCCESS;
}

static uint8_t tw_read(bool read_ack){
	if (read_ack){
		TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWEA);
		while (!(TWCR & (1 << TWINT)));
		if (TW_STATUS != TW_MR_DATA_ACK){
			return TW_STATUS;
		}
	}else{
		TWCR = (1 << TWINT) | (1 << TWEN);
		while (!(TWCR & (1 << TWINT)));
		if (TW_STATUS != TW_MR_DATA_NACK){
			return TW_STATUS;
		}
	}
	uint8_t data = TWDR;

	return data;
}


void i2c_init(i2c_freq freq, bool pullup_en){
	DDRC  |= (1 << TW_SDA_PIN) | (1 << TW_SCL_PIN);
	if (pullup_en)
	{
		PORTC |= (1 << TW_SDA_PIN) | (1 << TW_SCL_PIN);
	}
	else
	{
		PORTC &= ~((1 << TW_SDA_PIN) | (1 << TW_SCL_PIN));
	}
	DDRC  &= ~((1 << TW_SDA_PIN) | (1 << TW_SCL_PIN));
	
	switch (freq)
	{
		case TW_FREQ_100K:
		/* Set bit rate register 72 and prescaler to 1 resulting in
		SCL_freq = 16MHz/(16 + 2*72*1) = 100KHz	*/
		TWBR = 72;
		break;
		
		case TW_FREQ_250K:
		/* Set bit rate register 24 and prescaler to 1 resulting in
		SCL_freq = 16MHz/(16 + 2*24*1) = 250KHz	*/
		TWBR = 24;
		break;
		
		case TW_FREQ_400K:
		/* Set bit rate register 12 and prescaler to 1 resulting in
		SCL_freq = 16MHz/(16 + 2*12*1) = 400KHz	*/
		TWBR = 12;
		break;
		
		default: break;
	}
}


ret_code_t i2c_write(uint8_t slave_addr, uint8_t* p_data, uint8_t len, bool repeat_start, bool endianness)
{
	ret_code_t error_code;
	
	/* Send START condition */
	error_code = tw_start();
	if (error_code != SUCCESS)
	{
		return error_code;
	}
	
	/* Send slave address with WRITE flag */
	error_code = tw_write_sla(TW_SLA_W(slave_addr));
	if (error_code != SUCCESS)
	{
		return error_code;
	}
	
	/* Send data byte in single or burst mode */
	if(endianness == LITTLE_ENDIAN){
		for (int i = len - 1; i >= 0; i--){
			error_code = tw_write(p_data[i]);
#ifdef I2C_DEBUG			
			sprintf(i2c_print_buffer, "w[%d] 0x%X ", i, p_data[i]);
			serial_string(i2c_print_buffer);
#endif			
			if (error_code != SUCCESS){
				return error_code;
			}
		}		
	}else{
		for (int i = 0; i < len; ++i){
			error_code = tw_write(p_data[i]);
			if (error_code != SUCCESS){
				return error_code;
			}
		}
	}
#ifdef I2C_DEBUG
	serial_string("\n--\n");
#endif	
				
	if (repeat_start == REDO_START)
	{
		/* Send STOP condition */
		tw_stop();
	}
	
	return SUCCESS;
}


ret_code_t i2c_read(uint8_t slave_addr, uint8_t* p_data, uint8_t len, bool endianness){
	ret_code_t error_code;
	
	/* Send START condition */
	error_code = tw_start();
	if (error_code != SUCCESS)
	{
		return error_code;
	}
	
	/* Write slave address with READ flag */
	error_code = tw_write_sla(TW_SLA_R(slave_addr));
	if (error_code != SUCCESS)
	{
		return error_code;
	}
	
	/* Read single or multiple data byte and send ack */
	if(endianness == LITTLE_ENDIAN){
		for (uint8_t i = len-1; i > 0; i--){
			p_data[i] = tw_read(TW_READ_ACK);
#ifdef I2C_DEBUG			
			sprintf(bbb, "///\n%d -", p_data[i]);
#endif			
			if (error_code != SUCCESS){
				return error_code;
			}
		}
		p_data[0] = tw_read(TW_READ_NACK);
#ifdef I2C_DEBUG		
		sprintf(i2c_print_buffer+strlen(i2c_print_buffer), "%d\n////", p_data[0]);
		serial_string(i2c_print_buffer);
#endif		
	}else{ // BIG_ENDIAN
		for (uint8_t i = 0; i < len - 1; i++){
			p_data[i] = tw_read(TW_READ_ACK);
			if (error_code != SUCCESS){
				return error_code;
			}
		}
		p_data[len-1] = tw_read(TW_READ_NACK);
	}
	/* Send STOP condition */
	tw_stop();
	
	return SUCCESS;
}