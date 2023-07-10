/*
 * dc_motor.c
 *
 *  Created on: May 8, 2023
 *      Author: user
 */

#include "dc_motor.h"
#include "gpio.h"
#include "pwm.h"

/*
 * Description :function to initialize the Dc_motor driver
 * 				1.Set pin direction of motor
 * 				2.Stop motor at the beginning
 */
void DcMotor_Init(void)
{
	/* Setup the two motor pins as output pins */
	GPIO_setupPinDirection(MOTOR_FIRST_PIN_PORT_ID, MOTOR_FIRST_PIN_ID, PIN_OUTPUT);
	GPIO_setupPinDirection(MOTOR_SECOND_PIN_PORT_ID, MOTOR_SECOND_PIN_ID, PIN_OUTPUT);
	/* Stop motor at the beginning */
	GPIO_writePin(MOTOR_FIRST_PIN_PORT_ID, MOTOR_FIRST_PIN_ID, LOGIC_LOW);
	GPIO_writePin(MOTOR_SECOND_PIN_PORT_ID, MOTOR_SECOND_PIN_ID, LOGIC_LOW);
}

/*
 * Description :function to make motor move with certain state and speed
 * 				by PWM
 */
void DcMotor_Rotate(DcMotor_State state,uint8 speed)
{

	PWM_Timer0_Start(speed);
	if(speed >= MIN_SPEED_OF_MOTOR && speed <= MAX_SPEED_OF_MOTOR)
	{

		if(state == STOP)
		{
			GPIO_writePin(MOTOR_FIRST_PIN_PORT_ID, MOTOR_FIRST_PIN_ID, LOGIC_LOW);
			GPIO_writePin(MOTOR_SECOND_PIN_PORT_ID, MOTOR_SECOND_PIN_ID, LOGIC_LOW);
		}
		else if(state == CLOCKWISE)
		{
			GPIO_writePin(MOTOR_FIRST_PIN_PORT_ID, MOTOR_FIRST_PIN_ID, LOGIC_LOW);
			GPIO_writePin(MOTOR_SECOND_PIN_PORT_ID, MOTOR_SECOND_PIN_ID, LOGIC_HIGH);
		}
		else if(state == ANTI_CLOCKWISE)
		{
			GPIO_writePin(MOTOR_FIRST_PIN_PORT_ID, MOTOR_FIRST_PIN_ID, LOGIC_HIGH);
			GPIO_writePin(MOTOR_SECOND_PIN_PORT_ID, MOTOR_SECOND_PIN_ID, LOGIC_LOW);
		}
	}
}
