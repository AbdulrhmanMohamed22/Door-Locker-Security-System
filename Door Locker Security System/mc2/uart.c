/*
 * uart.c
 *
 *  Created on: Jun 21, 2023
 *      Author: user
 */

#include "uart.h"
#include "common_macros.h"
#include <avr/io.h>


void UART_init(const UART_ConfigType * Config_Ptr)
{
	uint16 baud_value;
	UCSRA = (1<<U2X);
	UCSRB = (1<<RXEN) | (1<<TXEN);
	UCSRC = (1<<URSEL);
	UCSRC =	(UCSRC & 0xCF)  | ((Config_Ptr->parity) <<4);
	UCSRC =	(UCSRC & 0xF7)  | ((Config_Ptr->stop_bit) <<3);
	UCSRC =	(UCSRC & 0xF9)  | ((Config_Ptr->bit_data) <<1);

	baud_value = (uint16)(((F_CPU/(Config_Ptr->baud_rate *8UL )))-1);
	UBRRL = baud_value;
	UBRRH = baud_value>>8;
}

void UART_sendByte(const uint8 data)
{
	while(BIT_IS_CLEAR(UCSRA,UDRE)){}
	UDR = data;

}

uint8 UART_recieveByte()
{
	while(BIT_IS_CLEAR(UCSRA,RXC)){}
	return UDR;
}

void UART_recieveString(uint8 *str)
{
	uint8 i=0;
	str[i] = UART_recieveByte();
	while(str[i] !='#')
	{
		i++;
		str[i] = UART_recieveByte();
	}
	str[i] = '\0';
}

void UART_sendString(uint8 *str)
{
	uint8 i=0;
	while(str[i] !='\0')
	{
		UART_sendByte(str[i]);
		i++;
	}

}
