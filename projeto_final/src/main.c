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

#define DELAY_USE_INTERRUPT			1

/**
 * \brief   This macro will check for write POSTED status
 *
 * \param   reg          Register whose status has to be checked
 *
 *    'reg' can take the following values \n
 *    DMTIMER_WRITE_POST_TCLR - Timer Control register \n
 *    DMTIMER_WRITE_POST_TCRR - Timer Counter register \n
 *    DMTIMER_WRITE_POST_TLDR - Timer Load register \n
 *    DMTIMER_WRITE_POST_TTGR - Timer Trigger register \n
 *    DMTIMER_WRITE_POST_TMAR - Timer Match register \n
 *
 **/
#define DMTimerWaitForWrite(reg)   \
            if(HWREG(DMTIMER_TSICR) & 0x4)\
            while((reg & HWREG(DMTIMER_TWPS)));
//

int flag_timer = false;
bool flag_gpio17 = false;

typedef enum _pinNum{
	PIN1=1,
	PIN2,
	PIN3, 
	PIN4,
	PIN5,
	PIN6
}pinNum;

void timerIrqHandler(void);
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
 *         Name:  putCh
 *  Description:  
 * =====================================================================================
 */
void putCh(char c){
	while(!(HWREG(UART0_LSR) & (1<<5)));

	HWREG(UART0_THR) = c;
}

/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  getCh
 *  Description:  
 * =====================================================================================
 */
char getCh(){
	while(!(HWREG(UART0_LSR) & (1<<0)));

	return(HWREG(UART0_RHR));
}

/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  putString
 *  Description:  
 * =====================================================================================
 */
int putString(char *str, unsigned int length){
	for(int i = 0; i < length; i++){
    	putCh(str[i]);
	}
	return(length);
}

/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  getString
 *  Description:  
 * =====================================================================================
 */
int getString(char *buf, unsigned int length){
	for(int i = 0; i < length; i ++){
    	buf[i] = getCh();
   	}
	return(length);
}

/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  timerEnable
 *  Description:  
 * =====================================================================================
 */
void timerEnable(){
    /* Wait for previous write to complete in TCLR */
	DMTimerWaitForWrite(0x1);

    /* Start the timer */
    HWREG(DMTIMER_TCLR) |= 0x1;
}/* -----  end of function timerEnable  ----- */

/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  timerDisable
 *  Description:  
 * =====================================================================================
 */
void timerDisable(){
    /* Wait for previous write to complete in TCLR */
	DMTimerWaitForWrite(0x1);

    /* Stop the timer */
    HWREG(DMTIMER_TCLR) &= ~(0x1);
}/* -----  end of function timerEnable  ----- */

/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  delay
 *  Description:  
 * =====================================================================================
 */
void delay(unsigned int mSec){
#ifdef DELAY_USE_INTERRUPT
    unsigned int countVal = TIMER_OVERFLOW - (mSec * TIMER_1MS_COUNT);

    /* Wait for previous write to complete */
	DMTimerWaitForWrite(0x2); //0010

    /* Load the register with the re-load value */
	HWREG(DMTIMER_TCRR) = countVal;
	
	flag_timer = false;

    /* Enable the DMTimer interrupts */
	HWREG(DMTIMER_IRQENABLE_SET) = 0x2; 

    /* Start the DMTimer */
	timerEnable();

	while(flag_timer == false);

    /* Disable the DMTimer interrupts */
	HWREG(DMTIMER_IRQENABLE_CLR) = 0x2; 
#else
    while(mSec != 0){
        /* Wait for previous write to complete */
        DMTimerWaitForWrite(0x2);

        /* Set the counter value. */
        HWREG(DMTIMER_TCRR) = 0x0;

        timerEnable();

        while(HWREG(DMTIMER_TCRR) < TIMER_1MS_COUNT);

        /* Stop the timer */
        HWREG(DMTIMER_TCLR) &= ~(0x00000001u);

        mSec--;
    }
#endif
}

/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  timerSetup
 *  Description:  
 * =====================================================================================
 */
void timerSetup(void){
     /*  Clock enable for DMTIMER7 TRM 8.1.12.1.25 */
    HWREG(CM_PER_TIMER7_CLKCTRL) |= 0x2;

	/*  Check clock enable for DMTIMER7 TRM 8.1.12.1.25 */    
    while((HWREG(CM_PER_TIMER7_CLKCTRL) & 0x3) != 0x2);

#ifdef DELAY_USE_INTERRUPT
    /* Interrupt mask */
    HWREG(INTC_MIR_CLEAR2) |= (1<<31);//(95 --> Bit 31 do 3º registrador (MIR CLEAR2))
#endif
}

/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  gpioSetup
 *  Description:  
 * =====================================================================================
 */
void gpioSetup(){
	/* set clock for GPIO1, TRM 8.1.12.1.31 */
	HWREG(CM_PER_GPIO1_CLKCTRL) = 0x40002; 

	// ==
	//HWREG(CM_PER_GPIO1_CLKCTRL) |= 0x40000;
	//HWREG(CM_PER_GPIO1_CLKCTRL) |= 0x2;

    /* Interrupt mask */


    HWREG(INTC_MIR_CLEAR3) |= (1<<2);//(98 --> Bit 2 do 4º registrador (MIR CLEAR3))
	HWREG(INTC_MIR_CLEAR3) |= (1<<3);//(99 --> Bit 3 do 4º registrador (MIR CLEAR3))
}
/* 98 >> 5 = 3
98/2
49/2
24/2
12/2
6/2
3 */

/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  butConfig
 *  Description:  
 * =====================================================================================
 */
void butConfig ( ){
    /*  configure pin mux for input GPIO */
    	HWREG(CM_PER_GPMCA1_REGS) |= 0x2F; // == (1<<5)|(1<<3)|(0x7); 10 1111
    
    	HWREG(GPIO1_OE) |= 1<<17;

    /* Setting interrupt GPIO pin. */
	HWREG(GPIO1_IRQSTATUS_SET_0) |= 1<<17;
	// associar um pino a determinado grupo de interrupção

  	/* Enable interrupt generation on detection of a rising edge.*/
	HWREG(GPIO1_RISINGDETECT) |= 1<<17;
}/* -----  end of function butConfig  ----- */

/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  ledConfig
 *  Description:  
 * =====================================================================================
 */
void ledConfig ( ){
    /*  configure pin mux for output GPIO */
	HWREG(CM_PER_GPMCAD12_REGS) |= 0x7;
	HWREG(CM_PER_GPMCAD13_REGS) |= 0x7;
	HWREG(CM_PER_GPMCAD14_REGS) |= 0x7;
	HWREG(CM_PER_GPMCAD15_REGS) |= 0x7;
	HWREG(CM_PER_GPMCA0_REGS) |= 0x7;
	HWREG(CM_PER_GPMCCSn0_REGS) |= 0x7;

	//DUVIDA: pq usar esses registradores a7 e a8?

    /* clear pin 23 and 24 for output, leds USR3 and USR4, TRM 25.3.4.3 */
	HWREG(GPIO1_OE) &= ~(1<<12);
	HWREG(GPIO1_OE) &= ~(1<<13);
	HWREG(GPIO1_OE) &= ~(1<<14);
	HWREG(GPIO1_OE) &= ~(1<<15);
	HWREG(GPIO1_OE) &= ~(1<<16);
	HWREG(GPIO1_OE) &= ~(1<<29);

}/* -----  end of function ledConfig  ----- */

/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  readBut
 *  Description:  
 * =====================================================================================
 */
unsigned int readBut ( ){
	unsigned int temp;
	temp = HWREG(GPIO1_DATAIN)&0x1000;
	
	return(temp);
}/* -----  end of function readBut  ----- */

/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  ledOff
 *  Description:  
 * =====================================================================================
 */
void ledOff(pinNum pin){
	switch (pin) {
		case PIN1:
			HWREG(GPIO1_CLEARDATAOUT) = (1<<12);
		break;
		case PIN2:	
			HWREG(GPIO1_CLEARDATAOUT) = (1<<13);
		break;
		case PIN3:	
			HWREG(GPIO1_CLEARDATAOUT) = (1<<14);
		break;
		case PIN4:	
			HWREG(GPIO1_CLEARDATAOUT) = (1<<15);
		break;
		case PIN5:	
			HWREG(GPIO1_CLEARDATAOUT) = (1<<16);
		break;
		case PIN6:	
			HWREG(GPIO1_CLEARDATAOUT) = (1<<29);
		break;
		default:
		break;
	}/* -----  end switch  ----- */
}

/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  ledOn
 *  Description:  
 * =====================================================================================
 */
void ledOn(pinNum pin){
	
	switch (pin) {
		case PIN1:
			HWREG(GPIO1_SETDATAOUT) |= (1<<12);
		break;
		case PIN2:
			HWREG(GPIO1_SETDATAOUT) |= (1<<13);
		break;
		case PIN3:
			HWREG(GPIO1_SETDATAOUT) |= (1<<14);
		break;
		case PIN4:
			HWREG(GPIO1_SETDATAOUT) |= (1<<15);
		break;
		case PIN5:
			HWREG(GPIO1_SETDATAOUT) |= (1<<16);
		break;
		case PIN6:
			HWREG(GPIO1_SETDATAOUT) |= (1<<29);
		break;
		default:
		break;
	}/* -----  end switch  ----- */
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
			HWREG(GPIO1_IRQSTATUS_0) = 0x20000; // == (1<<17) 10 0000 0000 0000 0000
			putString("button A press!\n\r",17);
			flag_gpio17 = true;
		break;
		default:
		break;
	}
}

/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  timerIrqHandler
 *  Description:  
 * =====================================================================================
 */
void timerIrqHandler(void){

    /* Clear the status of the interrupt flags */
	HWREG(DMTIMER_IRQSTATUS) = 0x2; 

	flag_timer = true;

    /* Stop the DMTimer */
	timerDisable();

}

/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  ISR_Handler
 *  Description:  
 * =====================================================================================
 */
void ISR_Handler(void){
	/* Verify active IRQ number */
	unsigned int irq_number = HWREG(INTC_SIR_IRQ) & 0x7f; // 110 0010 & 111 1111 = 11
	

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

	putString("gpio Interrupt...\n\r",19);
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




