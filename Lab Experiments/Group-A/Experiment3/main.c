#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/sysctl.h"
#include "driverlib/gpio.h"
#include "driverlib/debug.h"
#include "driverlib/pwm.h"
#include "driverlib/pin_map.h"
#include "inc/hw_gpio.h"
#include "driverlib/rom.h"


#define PWM_FREQUENCY 55

void setup(void)
{
	SysCtlClockSet(SYSCTL_SYSDIV_4|SYSCTL_USE_PLL|SYSCTL_XTAL_16MHZ|SYSCTL_OSC_MAIN);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
}

void led_pin_config(void)
{
	GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3);
}

int main(void)
{
	volatile uint32_t ui32Load;
	volatile uint32_t ui32PWMClock;
	volatile uint8_t ui8Adjust;
	ui8Adjust = 254;

	SysCtlClockSet(SYSCTL_SYSDIV_5|SYSCTL_USE_PLL|SYSCTL_OSC_MAIN|SYSCTL_XTAL_16MHZ);
	SysCtlPWMClockSet(SYSCTL_PWMDIV_64);

	SysCtlPeripheralEnable(SYSCTL_PERIPH_PWM1);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);

	GPIOPinTypePWM(GPIO_PORTF_BASE, GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3); // Changed here
	GPIOPinConfigure(GPIO_PF1_M1PWM5);
	GPIOPinConfigure(GPIO_PF2_M1PWM6);
	GPIOPinConfigure(GPIO_PF3_M1PWM7);

	HWREG(GPIO_PORTF_BASE + GPIO_O_LOCK) = GPIO_LOCK_KEY;
	HWREG(GPIO_PORTF_BASE + GPIO_O_CR) |= 0x01;
	HWREG(GPIO_PORTF_BASE + GPIO_O_LOCK) = 0;
	GPIODirModeSet(GPIO_PORTF_BASE, GPIO_PIN_4|GPIO_PIN_0, GPIO_DIR_MODE_IN);
	GPIOPadConfigSet(GPIO_PORTF_BASE, GPIO_PIN_4|GPIO_PIN_0, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPU);

	ui32PWMClock = SysCtlClockGet() / 64;
	ui32Load = (ui32PWMClock / PWM_FREQUENCY) - 1;
	PWMGenConfigure(PWM1_BASE, PWM_GEN_2, PWM_GEN_MODE_DOWN);
	PWMGenPeriodSet(PWM1_BASE, PWM_GEN_2, ui32Load);

	PWMGenConfigure(PWM1_BASE, PWM_GEN_3, PWM_GEN_MODE_DOWN);
	PWMGenPeriodSet(PWM1_BASE, PWM_GEN_3, ui32Load);


	PWMPulseWidthSet(PWM1_BASE, PWM_OUT_5, ui8Adjust * ui32Load / 1000);
	PWMPulseWidthSet(PWM1_BASE, PWM_OUT_6, ui8Adjust * ui32Load / 1000);
	PWMPulseWidthSet(PWM1_BASE, PWM_OUT_7, ui8Adjust * ui32Load / 1000);


	PWMOutputState(PWM1_BASE, PWM_OUT_5_BIT, true);
	PWMOutputState(PWM1_BASE, PWM_OUT_6_BIT, true);
	PWMOutputState(PWM1_BASE, PWM_OUT_7_BIT, true);

	PWMGenEnable(PWM1_BASE, PWM_GEN_2);
	PWMGenEnable(PWM1_BASE, PWM_GEN_3);


	int count = 0;
	int speed = 6000000;
	int temp = 500000;
	int mode = 0; //0 = auto, 1 = manual
	int pressed = 0;
	//int dp = 0;
	while(1)
	{
		if(mode == 0){
			uint8_t ui8LED = 2;
			if(count == 0){
				ui8LED = 2; //red
			}
			else if(count == 5){
				ui8LED = 6; //magenta
			}
			else if(count == 4){
				ui8LED = 4; //blue
			}
			else if(count == 3){
				ui8LED = 12; //cyan
			}
			else if(count == 2){
				ui8LED = 8; //green
			}
			else if(count == 1){
				ui8LED = 10; //yellow
			}

			setup();
			led_pin_config();

			GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3, ui8LED);

			if (count == 5)
			{
				count = 0;
			}
			else
			{
				count += 1;
			}

			if(GPIOPinRead(GPIO_PORTF_BASE,GPIO_PIN_4)==0x00)
			{
				speed -= temp;
				if(speed < 2000000){
					speed = 2000000;
				}
				//			ui8Adjust--;
				//			if (ui8Adjust < 10)
				//			{
				//				ui8Adjust = 10;
				//			}
				//			PWMPulseWidthSet(PWM1_BASE, PWM_OUT_5, ui8Adjust * ui32Load / 1000);
			}

			if(GPIOPinRead(GPIO_PORTF_BASE,GPIO_PIN_0)==0x00)
			{
				speed += temp;
				if(speed > 10000000){
					speed = 10000000;
				}
				//			ui8Adjust++;
				//			if (ui8Adjust > 254)
				//			{
				//				ui8Adjust = 254;
				//			}
				//			PWMPulseWidthSet(PWM1_BASE, PWM_OUT_5, ui8Adjust * ui32Load / 1000);
			}

			if(GPIOPinRead(GPIO_PORTF_BASE,GPIO_PIN_4)==0x00 && GPIOPinRead(GPIO_PORTF_BASE,GPIO_PIN_0)==0x00)
			{
				mode = 1; //switch to manual;
			}

			SysCtlDelay(speed);
		}
		else if(mode == 1){
			uint8_t ui8LED = 0;

			setup();
			led_pin_config();

			GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3, ui8LED);


			if(GPIOPinRead(GPIO_PORTF_BASE,GPIO_PIN_0)==0x00)
			{
				pressed = 1;
			}
			else
			{
				pressed = 0;
				//dp = 0;
			}

			if(pressed == 1)
			{

				if(GPIOPinRead(GPIO_PORTF_BASE,GPIO_PIN_4)==0x00)
				{
					mode = 2; //red led mode

				}
			}
			else
			{
				if(GPIOPinRead(GPIO_PORTF_BASE,GPIO_PIN_4)==0x00)
				{
					mode = 4; // green led mode
				}
			}

			/*
			if(GPIOPinRead(GPIO_PORTF_BASE,GPIO_PIN_4)==0x00 && GPIOPinRead(GPIO_PORTF_BASE,GPIO_PIN_0)==0x00)
			{
				mode = 4; //green led mode

			}
			else if(GPIOPinRead(GPIO_PORTF_BASE,GPIO_PIN_0)==0x00)
			{
				if(GPIOPinRead(GPIO_PORTF_BASE,GPIO_PIN_4)==0x00){
					mode = 2; //red led mode

				}
			}
		}*/
		}
		else if(mode == 2){
			//todo

			GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3, 2);
			SysCtlDelay(2000000);

			if(GPIOPinRead(GPIO_PORTF_BASE,GPIO_PIN_4)==0x00 && GPIOPinRead(GPIO_PORTF_BASE,GPIO_PIN_0)==0x00)
			{
				mode = 3; //blue led mode

			}




			if(GPIOPinRead(GPIO_PORTF_BASE,GPIO_PIN_4)==0x00)
			{
				ui8Adjust--;
				if (ui8Adjust < 10)
				{
					ui8Adjust = 10;
				}
				PWMPulseWidthSet(PWM1_BASE, PWM_OUT_5, ui8Adjust * ui32Load / 1000);
			}

			if(GPIOPinRead(GPIO_PORTF_BASE,GPIO_PIN_0)==0x00)
			{
				ui8Adjust++;
				if (ui8Adjust > 254)
				{
					ui8Adjust = 254;
				}
				PWMPulseWidthSet(PWM1_BASE, PWM_OUT_5, ui8Adjust * ui32Load / 1000);
			}




		}
		else if(mode == 3){
			//todo

			GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3, 4);
			SysCtlDelay(2000000);
			if(GPIOPinRead(GPIO_PORTF_BASE,GPIO_PIN_4)==0x00)
			{
				ui8Adjust--;
				if (ui8Adjust < 10)
				{
					ui8Adjust = 10;
				}
				PWMPulseWidthSet(PWM1_BASE, PWM_OUT_6, ui8Adjust * ui32Load / 1000);
			}

			if(GPIOPinRead(GPIO_PORTF_BASE,GPIO_PIN_0)==0x00)
			{
				ui8Adjust++;
				if (ui8Adjust > 254)
				{
					ui8Adjust = 254;
				}
				PWMPulseWidthSet(PWM1_BASE, PWM_OUT_6, ui8Adjust * ui32Load / 1000);
			}


		}
		else if(mode == 4){
			//todo

			GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3, 8);
			SysCtlDelay(2000000);
			if(GPIOPinRead(GPIO_PORTF_BASE,GPIO_PIN_4)==0x00)
			{
				ui8Adjust--;
				if (ui8Adjust < 10)
				{
					ui8Adjust = 10;
				}
				PWMPulseWidthSet(PWM1_BASE, PWM_OUT_7, ui8Adjust * ui32Load / 1000);
			}

			if(GPIOPinRead(GPIO_PORTF_BASE,GPIO_PIN_0)==0x00)
			{
				ui8Adjust++;
				if (ui8Adjust > 254)
				{
					ui8Adjust = 254;
				}
				PWMPulseWidthSet(PWM1_BASE, PWM_OUT_7, ui8Adjust * ui32Load / 1000);
			}

		}
	}
}
