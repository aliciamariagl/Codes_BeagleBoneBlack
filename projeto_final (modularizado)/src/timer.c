#include "timer.h"

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

int flag_timer = false;

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

    /* Start the timer */
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
	DMTimerWaitForWrite(0x2);

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