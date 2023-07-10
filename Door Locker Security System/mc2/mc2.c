/*-------------------------------------------------------------------------

  [FILE NAME]:     mc2.c

  [AUTHOR]:        Abdulrhman El-Sanhory

  [DATE CREATED]:  Jun 26, 2023

  [DESCRIPTION]:   Control_ECU with EEPROM, Buzzer, and Dc-Motor

--------------------------------------------------------------------------*/

/*-------------------------------INCLUDES---------------------------------*/
#include "uart.h"
#include "external_eeprom.h"
#include "twi.h"
#include "dc_motor.h"
#include "buzzer.h"
#include "timer1.h"
#include <util/delay.h>
#include <avr/io.h>

/*-------------------------------Definitions------------------------------*/
#define PASSWORD_LENGTH         5
#define ENTER_BUTTON           '='
#define MC2_READY              0x10
#define OPEN_DOOR              '+'
#define CHANGE_PASS            '-'
#define PASSWORD_ADDRESS       0x08
#define PASSWORD_MATCHED  	    1
#define PASSWORD_UNMATCHED	    0
#define DOOR_UNLOCKING    	 	5
#define HOLD_MOTOR				6
#define DOOR_LOCKING     	    11
#define MAX_NUMBER_OF_TRIALS    3

/*-----------------------------Global Variables---------------------------*/
uint8 g_count=0;
uint8 password_saved[PASSWORD_LENGTH];

/*---------------------------Functions Prototypes-------------------------*/
void receive_password(uint8 *pass,uint8 size);

uint8 password_match(uint8 *pass_1,uint8 *pass_2,uint8 size);

void save_password_In_EEPROM(uint8 *pass,uint8 size,uint8 address);

void get_password_In_EEPROM(uint8 *pass,uint8 size,uint8 address);

uint8 compare_password(uint8 *pass_1,uint8 *pass_2,uint8 size);

void timer_Processing();

uint8 do_option(uint8 *password,uint8 mission);

void motor();

void active_buzzer();

/*-----------------------------Main Function-------------------------------*/
int main(void)
{
	uint8 password[PASSWORD_LENGTH];
	uint8 password_reEntered[PASSWORD_LENGTH];

	SREG |=(1<<7);  /*Enable I-bit*/

	UART_ConfigType UART_config = {BIT_DATA_8,PARITY_DISABLED,STOP_BIT_1,BAUD_RATE_9600};
	UART_init(&UART_config);

	TWI_ConfigType TWI_config = {0x77,0x02,PRESCALER_1};
	TWI_init(&TWI_config);

	Timer1_setCallBack(timer_Processing);
	Timer1_ConfigType TIMWER_config = {42098,0,TIMER_PRESCALER_1024,OVERFLOW_MODE};

	DcMotor_Init();

	Buzzer_init();

	while(1)
	{
		/* receive password for first time */
		receive_password(password,PASSWORD_LENGTH);
		/* receive password for second time */
		receive_password(password_reEntered,PASSWORD_LENGTH);
		/* if two password matched -> save password in EEPROM and setup main option settings */
		if(password_match(password,password_reEntered,PASSWORD_LENGTH))
		{
			save_password_In_EEPROM(password,PASSWORD_LENGTH,PASSWORD_ADDRESS);
			get_password_In_EEPROM(password, PASSWORD_LENGTH, PASSWORD_ADDRESS);

			while(1)
			{
				uint8 receive_main_option_state = UART_recieveByte();
				Timer1_init(&TIMWER_config);
				if(receive_main_option_state == OPEN_DOOR)
				{
					do_option(password,OPEN_DOOR);

				}
				else if(receive_main_option_state == CHANGE_PASS)
				{
					uint8 number_of_trials = do_option(password,CHANGE_PASS);
					if(number_of_trials==1)
					{
						Timer1_deInit();
						break;
					}
				}
				Timer1_deInit();
			}
		}
	}
}


/*----------------------------Functions Definitions-------------------------*/

/*
 * Description : Function to receive password from second MicroController
 */
void receive_password(uint8 *pass,uint8 size)
{
	uint8 i;
	UART_sendByte(MC2_READY);
	for( i=0;i<PASSWORD_LENGTH;i++)
	{
		pass[i]=UART_recieveByte();
	}
}

/*
 * Description : Function to check if two entered password are matched or not
 */
uint8 password_match(uint8 *pass_1,uint8 *pass_2,uint8 size)
{
	uint8 i;
	for(i=0;i<size;i++)
	{
		if(pass_1[i] != pass_2[i])
		{
			UART_sendByte(PASSWORD_UNMATCHED);
			return FALSE;
		}
	}
	UART_sendByte((PASSWORD_MATCHED));
	return TRUE;
}

/*
 * Description : Function to save password in EEPROM
 */
void save_password_In_EEPROM(uint8 *pass,uint8 size,uint8 address)
{
	uint8 i;
	for(i=0;i<size;i++)
	{
		EEPROM_writeByte((address+i), pass[i]);
	}
}


/*
 * Description : Function to Get password in EEPROM
 */
void get_password_In_EEPROM(uint8 *pass,uint8 size,uint8 address)
{
	uint8 i;

	for(i=0;i<size;i++)
	{
		EEPROM_readByte((address+i),&pass[i]);
		password_saved[i] = pass[i];
	}
}



/*
 * Description : function to compare the two numbers
 * 				 return TRUE or FALSE
 */
uint8 compare_password(uint8 *pass_1,uint8 *pass_2,uint8 size)
{
	uint8 i;
	for(i=0;i<size;i++)
	{
		if(pass_1[i] != pass_2[i])
		{
			return FALSE;
		}
	}
	return TRUE;
}

/*
 * Description : function for timer process
 */
void timer_Processing()
{
	g_count++;
}

/*
 * Description : function to Do options open door or change password
 * 				 1.check for password if match or not
 * 				 2.open door -> rotate motor
 * 				 3.change password return to step 1
 * 				 4.Buzzer ON if password doesn't match
 */
uint8 do_option(uint8 *password,uint8 mission)
{
	uint8 number_of_trials=1;
	while(number_of_trials <= MAX_NUMBER_OF_TRIALS)
	{
		receive_password(password, PASSWORD_LENGTH);
		if(compare_password(password,password_saved,PASSWORD_LENGTH))
		{
			/* send password state */
			UART_sendByte(PASSWORD_MATCHED);
			if(mission == OPEN_DOOR)
			{
				motor();
			}
			number_of_trials=1;
			break;
		}else
		{
			UART_sendByte(PASSWORD_UNMATCHED);
			number_of_trials++;
		}
		if(number_of_trials > MAX_NUMBER_OF_TRIALS)
		{
			active_buzzer();
			_delay_ms(100);
		}
	}
	return number_of_trials;
}


/*
 * Description : function to rotate motor CW for 15 second
 * 				 then stop motor 3 second
 * 				 then rotate motor A-CW for 15 second
 */
void motor()
{
	g_count=0;
	/* rotates motor for 15-seconds CW */
	DcMotor_Rotate(CLOCKWISE, 100);
	while(g_count < DOOR_UNLOCKING);
	/* hold the motor for 3-second */
	DcMotor_Rotate(STOP, 0);
	while(g_count < HOLD_MOTOR);
	/* rotates motor for 15-seconds A-CW */
	DcMotor_Rotate(ANTI_CLOCKWISE, 100);
	while(g_count < DOOR_LOCKING);
	/* stop the motor */
	DcMotor_Rotate(STOP, 0);
}

/*
 * Description : function to active Buzzer for 1 min
 */
void active_buzzer()
{
	g_count=0;
	Buzzer_on();
	while(g_count<20);
	Buzzer_off();
}

