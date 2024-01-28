#ifndef _TIMER_H_
#define _TIMER_H_

#include "bbb_regs.h"
#include "hw_types.h"

void timerIrqHandler(void);
void timerEnable();
void timerDisable();
void delay(unsigned int);
void timerSetup(void);

#endif