/*
 * dc_motor.h
 *
 *  Created on: May 8, 2023
 *      Author: user
 */

#ifndef DC_MOTOR_H_
#define DC_MOTOR_H_

#include "std_types.h"
/*******************************************************************************
 *                                Definitions                                  *
 *******************************************************************************/

#define MOTOR_FIRST_PIN_PORT_ID              PORTB_ID
#define MOTOR_FIRST_PIN_ID                   PIN0_ID

#define MOTOR_SECOND_PIN_PORT_ID             PORTB_ID
#define MOTOR_SECOND_PIN_ID                  PIN1_ID

#define MIN_SPEED_OF_MOTOR                    0
#define QUARTER_SPEED_OF_MOTOR                25
#define HALF_SPEED_OF_MOTOR                   50
#define MAX_SPEED_OF_MOTOR                    100


/*******************************************************************************
 *                         Types Declaration                                   *
 *******************************************************************************/

typedef enum
{
	STOP,CLOCKWISE,ANTI_CLOCKWISE
}DcMotor_State;

/*******************************************************************************
 *                      Functions Prototypes                                   *
 *******************************************************************************/

void DcMotor_Init(void);
void DcMotor_Rotate(DcMotor_State state,uint8 speed);

#endif /* DC_MOTOR_H_ */
