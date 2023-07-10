 /******************************************************************************
 *
 * Module: GPIO
 *
 * File Name: gpio.h
 *
 * Description: Header file for the AVR GPIO driver
 *
 * Author: Mohamed Tarek
 *
 *******************************************************************************/

#ifndef GPIO_H_
#define GPIO_H_

#include "std_types.h"


#define NUM_OF_PORT 4
#define NUM_OF_PIN  8

#define PORTA_ID    0
#define PORTB_ID    1
#define PORTC_ID    2
#define PORTD_ID    3

#define PIN0_ID     0
#define PIN1_ID     1
#define PIN2_ID     2
#define PIN3_ID     3
#define PIN4_ID     4
#define PIN5_ID     5
#define PIN6_ID     6
#define PIN7_ID     7

// ??? why typedef
typedef enum{

	PIN_INPUT,PIN_OUTPUT
}Gpio_pinDirectionType;

typedef enum{

	PORT_INPUT,PORT_OUTPUT=0xFF
}Gpio_portDirectionType;


void GPIO_setupPinDirection(uint8 port_num,uint8 pin_num,Gpio_pinDirectionType direction);
uint8 GPIO_readPort(uint8 port_num);
uint8 GPIO_readPin(uint8 port_num,uint8 pin_num);
void GPIO_writePin(uint8 port_num,uint8 pin_num,uint8 value);
void GPIO_writePort(uint8 port_num,uint8 value);
void GPIO_setupPortDirection(uint8 port_num,Gpio_portDirectionType direction);

#endif /* GPIO_H_ */
