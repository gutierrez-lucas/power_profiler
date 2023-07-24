#ifndef __INA219_DEF_H
#define __INA219_DEF_H

typedef enum{
	CONFIGURATION_REGISTER  = 0x00,
	SHUNT_VOLTAGE_REGISTER	= 0x01,
	BUS_VOLTAGE_REGISTER	= 0x02,
	POWER_REGISTER			= 0x03,
	CURRENT_REGISTER		= 0x04,
	CALIBRATION_REGISTER	= 0x05
}ina219_register_t;

enum{
	INA219_CONFIG_BVOLTAGERANGE_16V =        (0x0000),  // 0-16V Range
	INA219_CONFIG_BVOLTAGERANGE_32V =        (0x2000),  // 0-32V Range
};

enum {
	INA219_CONFIG_BADCRES_9BIT       =       (0x0000),  // 9-bit bus res = 0..511
	INA219_CONFIG_BADCRES_10BIT      =       (0x0080),  // 10-bit bus res = 0..1023
	INA219_CONFIG_BADCRES_11BIT      =       (0x0100),  // 11-bit bus res = 0..2047
	INA219_CONFIG_BADCRES_12BIT       =      (0x0180),  // 12-bit bus res = 0..4097
};

enum {
	INA219_CONFIG_SADCRES_9BIT_1S_84US     = (0x0000),  // 1 x 9-bit shunt sample
	INA219_CONFIG_SADCRES_10BIT_1S_148US   = (0x0008),  // 1 x 10-bit shunt sample
	INA219_CONFIG_SADCRES_11BIT_1S_276US   = (0x0010),  // 1 x 11-bit shunt sample
	INA219_CONFIG_SADCRES_12BIT_1S_532US   = (0x0018),  // 1 x 12-bit shunt sample
	INA219_CONFIG_SADCRES_12BIT_2S_1060US  = (0x0048),	// 2 x 12-bit shunt samples averaged together
	INA219_CONFIG_SADCRES_12BIT_4S_2130US  = (0x0050),  // 4 x 12-bit shunt samples averaged together
	INA219_CONFIG_SADCRES_12BIT_8S_4260US  = (0x0058),  // 8 x 12-bit shunt samples averaged together
	INA219_CONFIG_SADCRES_12BIT_16S_8510US = (0x0060),  // 16 x 12-bit shunt samples averaged together
	INA219_CONFIG_SADCRES_12BIT_32S_17MS   = (0x0068),  // 32 x 12-bit shunt samples averaged together
	INA219_CONFIG_SADCRES_12BIT_64S_34MS   = (0x0070),  // 64 x 12-bit shunt samples averaged together
	INA219_CONFIG_SADCRES_12BIT_128S_69MS =  (0x0078),  // 128 x 12-bit shunt samples averaged together
};

enum {
	INA219_CONFIG_MODE_POWERDOWN          =  (0x0000),
	INA219_CONFIG_MODE_SVOLT_TRIGGERED    =  (0x0001),
	INA219_CONFIG_MODE_BVOLT_TRIGGERED    =  (0x0002),
	INA219_CONFIG_MODE_SANDBVOLT_TRIGGERED = (0x0003),
	INA219_CONFIG_MODE_ADCOFF             =  (0x0004),
	INA219_CONFIG_MODE_SVOLT_CONTINUOUS  =  (0x0005),
	INA219_CONFIG_MODE_BVOLT_CONTINUOUS   =  (0x0006),
	INA219_CONFIG_MODE_SANDBVOLT_CONTINUOUS = (0x0007),
};

typedef enum{
	CALIBRATION_40mV_400mA	= (33573),
	CALIBRATION_80mV_800mA	= (16777),
	CALIBRATION_160mV_1A6	= (8388),
	CALIBRATION_320mV_2A	= (4194)
}ina219_calibration_t;	

typedef enum{
	INA219_CONFIG_GAIN_1_40mV_400mA        =       (0x0000),  
	INA219_CONFIG_GAIN_2_80mV_800mA        =       (0x0800),  
	INA219_CONFIG_GAIN_4_160mV_1A6		   =       (0x1000),  
	INA219_CONFIG_GAIN_8_320mV_2A	       =       (0x1800),  
}ina219_gain_t;
#endif