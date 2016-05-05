#include <stdint.h>
#include <stdbool.h>
#include "inc/tm4c123gh6pm.h"
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/sysctl.h"
#include "driverlib/interrupt.h"
#include "driverlib/gpio.h"
#include "driverlib/timer.h"


// LOCK_F and CR_F - used for unlocking PORTF pin 0
#define LOCK_F (*((volatile unsigned long *)0x40025520))
#define CR_F   (*((volatile unsigned long *)0x40025524))


_Bool sw_flag=false;
_Bool sw1_key=true;

uint8_t led=2;


_Bool sw_flag_1=false;
_Bool sw1_key_1=true;


int sw2Status=0;

void switchPinConfig(void)
{
	// Following two line removes the lock from SW2 interfaced on PORTF Pin0 -- leave this unchanged
	LOCK_F=0x4C4F434BU;
	CR_F=GPIO_PIN_0|GPIO_PIN_4;

	// GPIO PORTF Pin 0 and Pin4
	GPIODirModeSet(GPIO_PORTF_BASE,GPIO_PIN_4|GPIO_PIN_0,GPIO_DIR_MODE_IN); // Set Pin-4 of PORT F as Input. Modifiy this to use another switch
	GPIOPinTypeGPIOInput(GPIO_PORTF_BASE, GPIO_PIN_4|GPIO_PIN_0);
	GPIOPadConfigSet(GPIO_PORTF_BASE,GPIO_PIN_4|GPIO_PIN_0,GPIO_STRENGTH_12MA,GPIO_PIN_TYPE_STD_WPU);
}



// Part 1

void idle()
{
	if(sw_flag)
	{
		if(led == 2)
		{
			led = 8;
		}
		else if(led == 8)
		{
			led = 4;
		}
		else if(led == 4)
		{
			led = 2;
		}
		sw_flag = false;
	}
	//GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3, 14);


}

void press()
{
	//GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3, 6);

}

void cont()
{
	if(!sw_flag)
	{
		GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3, led);
		sw_flag = true;
	}


}



// Part 2


void idle_1()
{
	if(sw_flag_1)
	{
		sw_flag_1 = false;
	}
	//GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3, 14);


}

void press_1()
{
	//GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3, 6);

}

void cont_1()
{
	if(!sw_flag_1)
	{
		//GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3, led);
		sw2Status ++ ;
		sw_flag_1 = true;
	}


}





unsigned char detectKeyPress(void)
{
	unsigned char flag;

	//GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3, 6);

	if(GPIOPinRead(GPIO_PORTF_BASE, GPIO_PIN_4) == 0x00)
	{

		if(sw1_key)
		{
			press();
			sw1_key = false;
		}
		else
		{
			cont();
			flag = 1;
		}

	}
	else
	{
		idle();
		sw1_key = true;
	}

	// Part 2

	if(GPIOPinRead(GPIO_PORTF_BASE, GPIO_PIN_0) == 0x00)
		{

			if(sw1_key_1)
			{
				press_1();
				sw1_key_1 = false;
			}
			else
			{
				cont_1();
				flag = 1;
			}

		}
		else
		{
			idle_1();
			sw1_key_1 = true;
		}

	return flag;

}


void Timer0IntHandler(void)
{
	// Clear the timer interrupt
	TimerIntClear(TIMER0_BASE, TIMER_TIMA_TIMEOUT);
	// Read the current state of the GPIO pin and
	// write back the opposite state
	/*
	if(GPIOPinRead(GPIO_PORTF_BASE, GPIO_PIN_2))
	{
		GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3, 0);
	}
	else
	{
		GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_2, 4);
	}
	 */
	detectKeyPress();
}



int main(void)
{
	uint32_t ui32Period;


	SysCtlClockSet(SYSCTL_SYSDIV_5|SYSCTL_USE_PLL|SYSCTL_XTAL_16MHZ|SYSCTL_OSC_MAIN);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);

	GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3);

	SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER0);
	TimerConfigure(TIMER0_BASE, TIMER_CFG_PERIODIC);

	ui32Period = (SysCtlClockGet() / 1) / 2;

	TimerLoadSet(TIMER0_BASE, TIMER_A, ui32Period -1);

	IntEnable(INT_TIMER0A);

	TimerIntEnable(TIMER0_BASE, TIMER_TIMA_TIMEOUT);
	IntMasterEnable();
	switchPinConfig();
	TimerEnable(TIMER0_BASE, TIMER_A);
	while(1)
	{
	}
}

