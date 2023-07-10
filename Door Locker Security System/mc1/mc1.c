/*-------------------------------------------------------------------------

  [FILE NAME]:     mc1.c

  [AUTHOR]:        Abdulrhman El-Sanhory

  [DATE CREATED]:  Jun 26, 2023

  [DESCRIPTION]:   HMI_ECU (Human Machine Interface) with 2x16 LCD and 4x4 keypad

--------------------------------------------------------------------------*/

/*-------------------------------INCLUDES---------------------------------*/
#include "lcd.h"
#include "keypad.h"
#include "uart.h"
#include "timer1.h"
#include <util/delay.h>
#include <avr/io.h>

/*-------------------------------Definitions------------------------------*/
#define PASSWORD_LENGTH        5
#define ENTER_BUTTON          '='
#define OPEN_DOOR             '+'
#define CHANGE_PASS           '-'
#define MC2_READY             0x10
#define PASSWORD_MATCHED       1
#define DOOR_UNLOCKING     	   5
#define HOLD_MOTOR			   6
#define DOOR_LOCKING 		   11
#define MAX_NUMBER_OF_TRIALS   3

/*-----------------------------Global Variables---------------------------*/
uint8 g_count=0;

/*---------------------------Functions Prototypes-------------------------*/

void enter_password(uint8 *password,uint8 size);

void reEnter_password(uint8* password,uint8 size);

void send_password(uint8 *pass);

void main_options();

uint8 do_option(uint8 *password,uint8 mission);

void timer_Processing();

void display_motor_statue();

void error_message();

/*-----------------------------Main Function-------------------------------*/
int main(void)
{

	/* two arrays to carry the password in first and second time */
	uint8 password[PASSWORD_LENGTH];
	uint8 password_reEntered[PASSWORD_LENGTH];

	SREG |= (1<<7); /*Enable I-bit*/

	/* Initialize UART */
	UART_ConfigType uart_config = {BIT_DATA_8,PARITY_DISABLED,STOP_BIT_1,BAUD_RATE_9600};
	UART_init(&uart_config);

	/* Initialize TIMER1 */
	Timer1_setCallBack(timer_Processing);
	Timer1_ConfigType TIMER_config = {42098,0,TIMER_PRESCALER_1024,OVERFLOW_MODE};
	//	Timer1_init(&TIMER_config);

	/* Initialize LCD */
	LCD_init();

	while(1)
	{
		/* Enter the password for the system for first time */
		enter_password(password,PASSWORD_LENGTH);
		send_password(password);
		/* Enter the same password for the system for second time and send it */
		reEnter_password(password_reEntered,PASSWORD_LENGTH);
		send_password(password_reEntered);
		/* If two password matched then Display Main Options */
		if(UART_recieveByte() == PASSWORD_MATCHED)
		{
			while(1)
			{
				main_options();
				if(KEYPAD_getPressedKey() == OPEN_DOOR)
				{
					UART_sendByte(OPEN_DOOR);
					Timer1_init(&TIMER_config);
					do_option(password,OPEN_DOOR);
				}
				else if(KEYPAD_getPressedKey() == CHANGE_PASS)
				{
					UART_sendByte(CHANGE_PASS);
					Timer1_init(&TIMER_config);
					uint8 number_of_trials = do_option(password, CHANGE_PASS);
					if(number_of_trials == 1)
					{
						Timer1_deInit();
						// to return to step 1
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
 * Description : function to Display enter password message and password
 */
void enter_password(uint8 *password,uint8 size)
{
	uint8 i;
	LCD_clearScreen();
	LCD_displayString("plz enter pass:");
	LCD_moveCursor(1, 0);
	for(i=0;i<size;i++)
	{
		password[i] = KEYPAD_getPressedKey();
		LCD_displayCharacter('*');
		_delay_ms(500);
	}
	/* wait for ENTER BUTTON to pressed */
	while(!(KEYPAD_getPressedKey() == ENTER_BUTTON));
}

/*
 * Description : function to Display enter password message and password
 * 				 for second time
 */
void reEnter_password(uint8* password,uint8 size)
{
	uint8 i;
	LCD_clearScreen();
	LCD_displayString("plz re-enter the");
	LCD_displayStringRowColumn(1, 0, "same pass:");
	LCD_moveCursor(1, 11);
	for(i=0;i<PASSWORD_LENGTH;i++)
	{
		password[i] = KEYPAD_getPressedKey();
		LCD_displayCharacter('*');
		_delay_ms(500);
	}
	/* wait for ENTER BUTTON to pressed */
	while(!(KEYPAD_getPressedKey() == ENTER_BUTTON));
}

/*
 * Description : function to send password message to other MC2
 *
 */
void send_password(uint8 *pass)
{
	uint8 i ;
	while(UART_recieveByte() != MC2_READY);
	for(i=0;i<PASSWORD_LENGTH;i++)
	{
		UART_sendByte(pass[i]);
		_delay_ms(50);
	}
}

/*
 * Description : function to Display main option on LCD
 */
void main_options()
{
	LCD_clearScreen();
	LCD_displayStringRowColumn(0,0,"+ : Open Door");
	LCD_displayStringRowColumn(1,0,"- : Change Pass");
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
 * 				 2.open door -> display motor status
 * 				 3.change password return to step 1
 * 				 4.warning message if password doesn't match
 */
uint8 do_option(uint8 *password,uint8 mission)
{
	uint8 number_of_trials=1;
	uint8 password_state;
	_delay_ms(500);
	while(number_of_trials <= MAX_NUMBER_OF_TRIALS)
	{
		enter_password(password,PASSWORD_LENGTH);
		send_password(password);
		password_state = UART_recieveByte();
		/*if the password match then open the door*/
		if(password_state == TRUE)
		{
			if(mission == OPEN_DOOR)
			{
				display_motor_statue();
			}
			number_of_trials=1;
			break;
		}
		else
		{
			number_of_trials++;
		}
		if(number_of_trials>MAX_NUMBER_OF_TRIALS)
		{
			/* number of tries> Max number of tries -> Display error message for 1 min */
			error_message();
		}
	}
	return number_of_trials;
}

/*
 * Description : function to Display Door statue (Unlocking - Opened - Locking)
 */
void display_motor_statue()
{
	LCD_clearScreen();
	g_count=0;
	/* Open the Door in 15 second */
	LCD_displayStringRowColumn(0, 5, "Door is");
	LCD_displayStringRowColumn(1, 4, "Unlocking");
	while(g_count < DOOR_UNLOCKING);
	/* Hold the Door for 3 second */
	LCD_clearScreen();
	LCD_displayStringRowColumn(0, 5, "Door is");
	LCD_displayStringRowColumn(1, 5, "Opened");
	while(g_count < HOLD_MOTOR);
	/* Close the Door in 15 second */
	LCD_clearScreen();
	LCD_displayStringRowColumn(0, 5, "Door is");
	LCD_displayStringRowColumn(1, 5, "Locking");
	while(g_count < DOOR_LOCKING);
}

/*
 * Description : function to display error message on LCD for 1 min
 */
void error_message()
{
	g_count=0;
	LCD_clearScreen();
	LCD_displayStringRowColumn(0, 4, "WARNING!");
	while(g_count<20);
	LCD_clearScreen();
	_delay_ms(100);
}
