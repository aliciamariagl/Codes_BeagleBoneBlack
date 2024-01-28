#include "gpio.h"

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
}

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