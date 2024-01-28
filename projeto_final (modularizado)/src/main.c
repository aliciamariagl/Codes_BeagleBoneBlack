/*
 * =====================================================================================
 *
 *       Filename:  main.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  15/05/2018 14:32:47
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Francisco Helder (FHC), helderhdw@gmail.com
 *   Organization:  UFC-Quixadá
 *
 * =====================================================================================
 */

#include "bbb_regs.h"
#include "hw_types.h"
#include "timer.h"
#include "uart.h"
#include "gpio.h"

bool flag_gpio17;

/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  disableWdt
 *  Description:  
 * =====================================================================================
 */
void disableWdt(void){
	HWREG(WDT_WSPR) = 0xAAAA;
	while((HWREG(WDT_WWPS) & (1<<4)));
	
	HWREG(WDT_WSPR) = 0x5555;
	while((HWREG(WDT_WWPS) & (1<<4)));
}

/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  gpioIsrHandler
 *  Description:  
 * =====================================================================================
 */
void gpioIsrHandler(unsigned int irq_number){

	switch (irq_number){
		case 95:
			timerIrqHandler();
		break;
		case 98:
			HWREG(GPIO1_IRQSTATUS_0) = 0x20000; 
			putString("motion detected!\n\r",18);
			flag_gpio17 = true;
		break;
		default:
		break;
	}
}

/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  ISR_Handler
 *  Description:  
 * =====================================================================================
 */
void ISR_Handler(void){
	/* Verify active IRQ number */
	unsigned int irq_number = HWREG(INTC_SIR_IRQ) & 0x7f; 
	

	gpioIsrHandler(irq_number);
    
	/* acknowledge IRQ */
	HWREG(INTC_CONTROL) = 0x1;
}

/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  main
 *  Description:  
 * =====================================================================================
 */
int main(void){
	int cont = 0;

	/* Hardware setup */
	gpioSetup();
	timerSetup();
	ledConfig();
	butConfig();
	disableWdt();
	flag_gpio17 = false;

	putString("gpio Interrupt...\n\r",19);
	putString("placa ok\n\r",10);
	ledOff(PIN1); // VERMELHO CARRO
	ledOff(PIN2); // AMARELO CARRO
	ledOff(PIN3); // VERDE CARRO
	ledOff(PIN4); // VERMELHO PEDESTRE
	ledOff(PIN5); // VERDE PEDESTRE   
	ledOff(PIN6); // VERDE PARA DEFICIENTES VISUAIS --> buzzer

	while(true){
		if(flag_gpio17){
			ledOff(PIN3);
			ledOn(PIN2);
			delay(3000);
			ledOff(PIN2);
			ledOn(PIN1);
			ledOff(PIN4);
			ledOn(PIN5);
			ledOn(PIN6);
			delay(5000);
			cont = 1;
			flag_gpio17 = false;
		}else{
			ledOff(PIN1);
			ledOn(PIN3);
			ledOff(PIN5);
			ledOn(PIN4);
			ledOff(PIN6);
			if(cont == 1){
				delay(5000);
				cont = 0;
			}
		}
	}

	return(0);
}
