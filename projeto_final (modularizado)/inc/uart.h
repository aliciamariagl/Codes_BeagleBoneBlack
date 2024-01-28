#ifndef _UART_H_
#define _UART_H_

#include "bbb_regs.h"
#include "hw_types.h"

void putCh(char);
char getCh();
int putString(char *, unsigned int);
int getString(char *, unsigned int);

#endif