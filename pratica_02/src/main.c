		/*
 * =====================================================================================
 *
 *       Filename:  main.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  12/02/2017 20:05:55
 *       Revision:  none
 *       Compiler:  arm-none-eabi-gcc
 *
 *         Author:  Francisco Helder (FHC), helderhdw@gmail.com
 *   Organization:  UFC-Quixadá
 *
 * =====================================================================================
 */

#include	"hw_types.h"
#include	"soc_AM335x.h"

/*****************************************************************************
**                INTERNAL MACRO DEFINITIONS
*****************************************************************************/
#define TIME												1000000
#define TOGGLE          										(0x01u)

#define CM_PER_GPIO1											0xAC
#define CM_PER_GPIO1_CLKCTRL_MODULEMODE_ENABLE   							(0x2u)
#define CM_PER_GPIO1_CLKCTRL_OPTFCLKEN_GPIO_1_GDBCLK   						(0x00040000u)

#define CM_PER_GPIO2                                                                                 0xB0
#define CM_PER_GPIO2_CLKCTRL_MODULEMODE_ENABLE                                                       (0x2u)        
#define CM_PER_GPIO2_CLKCTRL_OPTFCLKEN_GPIO_2_GDBCLK                                                 (0x00040000u)
                                                                                                                   
#define CM_conf_gpmc_ben1      	 								0x0878
#define CM_conf_gpmc_a5         								0x0854

#define GPIO_OE                 								0x134
#define GPIO_CLEARDATAOUT       								0x190
#define GPIO_SETDATAOUT         								0x194

#define VAR(x)                                                                              (1<<x)        
#define LED1                                                                                 28                                                                 
#define LED2                                                                                 1



unsigned int flagBlink;

/*****************************************************************************
**                INTERNAL FUNCTION PROTOTYPES
*****************************************************************************/
static void delay();
static void ledInit();
static void ledToggle();
void inverso();

/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  main
 *  Description:  
 * =====================================================================================
 */
int _main(void){

	flagBlink=0;	//init flag
  	
	/* Configure the green LED control pin. */
  	ledInit();
  
  	while (1){
    	/* Change the state of the green LED. */
    		ledToggle();
		inverso();
		
	}

	return(0);
} /* ----------  end of function main  ---------- */


/*FUNCTION*-------------------------------------------------------
*
* Function Name : Delay
* Comments      :
*END*-----------------------------------------------------------*/
static void delay(){
	volatile unsigned int ra;
	for(ra = 0; ra < TIME; ra ++);
}


/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  ledInit
 *  Description:  
 * =====================================================================================
 */
void ledInit( ){
	
	unsigned int val_temp; 	
	/*-----------------------------------------------------------------------------
	 *  configure clock GPIO in clock module
	 *-----------------------------------------------------------------------------*/
	HWREG(SOC_CM_PER_REGS+CM_PER_GPIO1) |= CM_PER_GPIO1_CLKCTRL_OPTFCLKEN_GPIO_1_GDBCLK | CM_PER_GPIO1_CLKCTRL_MODULEMODE_ENABLE;
	HWREG(SOC_CM_PER_REGS+CM_PER_GPIO2) |= CM_PER_GPIO2_CLKCTRL_OPTFCLKEN_GPIO_2_GDBCLK | CM_PER_GPIO2_CLKCTRL_MODULEMODE_ENABLE;
	
	/*-----------------------------------------------------------------------------
	 * configure mux pin in control module
	 *-----------------------------------------------------------------------------*/
   	HWREG(SOC_CONTROL_REGS+CM_conf_gpmc_a5) |= 7;
 
	/*-----------------------------------------------------------------------------
	 *  set pin direction 
	 *-----------------------------------------------------------------------------*/
	val_temp = HWREG(SOC_GPIO_1_REGS+GPIO_OE);
	val_temp &= ~(1<<21);
	val_temp &= ~(1<<22);
	val_temp &= ~(1<<23);
	val_temp &= ~(1<<24);
	val_temp &= ~(1<<LED1);
	HWREG(SOC_GPIO_1_REGS+GPIO_OE) = val_temp;
	
	val_temp = HWREG(SOC_GPIO_2_REGS+GPIO_OE);
	val_temp &= ~(1<<LED2);
	HWREG(SOC_GPIO_2_REGS+GPIO_OE) = val_temp;
	
		
}/* -----  end of function ledInit  ----- */



/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  ledToggle
 *  Description:  
 * =====================================================================================
 */
void ledToggle( ){

		for(int i=21; i<25; i++){
			HWREG(SOC_GPIO_1_REGS+GPIO_SETDATAOUT) = VAR(i);
			delay();
			HWREG(SOC_GPIO_1_REGS+GPIO_CLEARDATAOUT) = VAR(i);
			delay();
		}
		HWREG(SOC_GPIO_1_REGS+GPIO_SETDATAOUT) = (1<<28);
		delay();
		HWREG(SOC_GPIO_1_REGS+GPIO_CLEARDATAOUT) = (1<<LED1);
		delay();
		HWREG(SOC_GPIO_2_REGS+GPIO_SETDATAOUT) = (1<<LED2);
		delay();
		HWREG(SOC_GPIO_2_REGS+GPIO_CLEARDATAOUT) = (1<<LED2);
		delay();
		
		/*flagBlink ^= TOGGLE;

		if(flagBlink){
			HWREG(SOC_GPIO_1_REGS+GPIO_SETDATAOUT) = (1<<LED1);
			HWREG(SOC_GPIO_2_REGS+GPIO_SETDATAOUT) = (1<<LED2);
		}else{
			HWREG(SOC_GPIO_1_REGS+GPIO_CLEARDATAOUT) = (1<<LED1);
			HWREG(SOC_GPIO_2_REGS+GPIO_CLEARDATAOUT) = (1<<LED2);
		}*/
}

void inverso(){
		HWREG(SOC_GPIO_2_REGS+GPIO_SETDATAOUT) = (1<<LED2);
		delay();
		HWREG(SOC_GPIO_2_REGS+GPIO_CLEARDATAOUT) = (1<<LED2);
		delay();
		HWREG(SOC_GPIO_1_REGS+GPIO_SETDATAOUT) = (1<<LED1);
		delay();
		HWREG(SOC_GPIO_1_REGS+GPIO_CLEARDATAOUT) = (1<<LED1);
		delay();
		for(int i=24; i>=21; i--){
			HWREG(SOC_GPIO_1_REGS+GPIO_SETDATAOUT) = VAR(i);
			delay();
			HWREG(SOC_GPIO_1_REGS+GPIO_CLEARDATAOUT) = VAR(i);
			delay();
		}
}
	
/* -----  end of function ledToggle  ----- */


//      HWREG(SOC_GPIO_1_REGS+GPIO_DATAIN) &= ( 1 << 28 )
	



                   


