/*
 * timer1.c
 *
 *  Created on: Jun 22, 2023
 *      Author: user
 */


#include "timer1.h"
#include <avr/io.h>
#include <avr/interrupt.h>

/*******************************************************************************
 *                           Global Variables                                  *
 *******************************************************************************/

/* Global variables to hold the address of the call back function in the application */
static volatile void (*g_callBackptr)(void) = NULL_PTR;
uint16 timer_value;

/*******************************************************************************
 *                       Interrupt Service Routines                            *
 *******************************************************************************/
ISR(TIMER1_OVF_vect)
{
	if(g_callBackptr != NULL_PTR)
	{
		TCNT1=timer_value;
		(*g_callBackptr)();
	}
}

ISR(TIMER1_COMPA_vect)
{
	if(g_callBackptr != NULL_PTR)
	{
		TCNT1=timer_value;
		(*g_callBackptr)();
	}
}

/*******************************************************************************
 *                   Functions Definitions                                     *
 *******************************************************************************/
/*
 * Description : Function to initialize the Timer driver
 */
void Timer1_init(const Timer1_ConfigType * Config_Ptr)
{

	TCNT1 = Config_Ptr->initial_value;
	timer_value = Config_Ptr->initial_value;
	if(Config_Ptr->mode == OVERFLOW_MODE)
	{
		TCCR1A = (1<<FOC1A) | (1<<FOC1B);
		TIMSK |= (1<<TOIE1);
	}
	else if (Config_Ptr->mode == COMPARE_MODE)
	{
		OCR1A = Config_Ptr->compare_value;
		TCCR1A =(1<<FOC1A) | (1<<FOC1B) | (1<<WGM12);
		TIMSK |= (1<<OCIE1A); // Enable Timer0 Overflow Interrupt
	}
	TCCR1B = (TCCR1B & 0xF8) | (Config_Ptr->prescaler);

}

/*
 * Description : Function to disable the Timer1.
 */
void Timer1_deInit(void)
{
	TCCR1B = 0;
	OCR1A = 0;
	TIMSK = 0;
}


/*
 * Description : Function to set the Call Back function address.
 */
void Timer1_setCallBack(void(*a_ptr)(void))
{
	g_callBackptr = a_ptr;
}


