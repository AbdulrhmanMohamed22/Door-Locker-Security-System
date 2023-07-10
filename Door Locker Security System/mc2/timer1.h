/*
 * timer1.h
 *
 *  Created on: Jun 22, 2023
 *      Author: user
 */

#ifndef TIMER1_H_
#define TIMER1_H_


#include "std_types.h"

typedef enum
{
	TIMER_NO_CLOCK,TIMER_NO_PRESCALER,TIMER_PRESCALER_8,TIMER_PRESCALER_64,TIMER_PRESCALER_256,TIMER_PRESCALER_1024
}Timer1_Prescaler;


typedef enum
{
	OVERFLOW_MODE,COMPARE_MODE
}Timer1_Mode;

typedef struct {
 uint16 initial_value;
 uint16 compare_value; // it will be used in compare mode only.
 Timer1_Prescaler prescaler;
 Timer1_Mode mode;
} Timer1_ConfigType;

/*
 * Description : Function to initialize the Timer driver
 */
void Timer1_init(const Timer1_ConfigType * Config_Ptr);

/*
 * Description : Function to disable the Timer1.
 */
void Timer1_deInit(void);

/*
 * Description : Function to set the Call Back function address.
 */
void Timer1_setCallBack(void(*a_ptr)(void));




#endif /* TIMER1_H_ */
