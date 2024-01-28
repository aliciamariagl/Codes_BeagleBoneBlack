#ifndef _GPIO_H_
#define _GPIO_H_

#include "bbb_regs.h"
#include "hw_types.h"

typedef enum _pinNum{
	PIN1=1,
	PIN2,
	PIN3, 
	PIN4,
	PIN5,
	PIN6
}pinNum;

void gpioSetup();
void butConfig();
void ledConfig();
unsigned int readBut();
void ledOff(pinNum);
void ledOn(pinNum);

#endif