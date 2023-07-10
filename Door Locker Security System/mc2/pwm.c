/*
 * timer.c
 *
 *  Created on: May 8, 2023
 *      Author: user
 */



#include "pwm.h"
#include <avr/io.h>
#include "gpio.h"



/*******************************************************************************
 *                      Functions Definitions                                  *
 *******************************************************************************/
/*
 * Description: Function to use PWM of timer 0
 * takes input duty cycle
 * return void
 * */

void PWM_Timer0_Start(uint8 duty_cycle)
{

	TCNT0=0;
	/* to calculate duty cycle as percentage */
	OCR0=((float)duty_cycle/100) * 255;
	GPIO_setupPinDirection(PORTB_ID, PIN3_ID, PIN_OUTPUT);
	/*
	 * TCCR0 register:
	 * FOC0: if it's not PWM
	 * WGM00,WGM01 : to choose the mode
	 * COM00,COM01 : choose output compare pin inverting or non
	 * CS00:CS02 : for Prescaler
	 * */
	TCCR0 =(1<<WGM01) |(1<<WGM00) | (1<<COM01) | (1<<CS01);

}
