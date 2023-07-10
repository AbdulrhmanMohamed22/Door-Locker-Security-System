/*
 * uart.h
 *
 *  Created on: Jun 21, 2023
 *      Author: user
 */

#ifndef UART_H_
#define UART_H_


#include "std_types.h"



typedef enum
{
	BIT_DATA_5,BIT_DATA_6,BIT_DATA_7,BIT_DATA_8,BIT_DATA_9=7
}UART_BitData;

typedef enum
{
	PARITY_DISABLED,PARITY_RESERVED,PARITY_EVEN,PARITY_ODD

}UART_Parity;

typedef enum
{
	STOP_BIT_1,STOP_BIT_2
}UART_StopBit;

typedef enum
{
	BAUD_RATE_4800=4800,BAUD_RATE_9600=9600,BAUD_RATE_14400=14400,BAUD_RATE_19200=19200

}UART_BaudRate;


typedef struct
{
	UART_BitData bit_data;
	UART_Parity parity;
	UART_StopBit stop_bit;
	UART_BaudRate baud_rate;
}UART_ConfigType;


void UART_init(const UART_ConfigType *Config_Ptr);

void UART_sendByte(const uint8 data);

uint8 UART_recieveByte();

void UART_recieveString(uint8 *Str);

void UART_sendString(uint8 *str);

#endif /* UART_H_ */
