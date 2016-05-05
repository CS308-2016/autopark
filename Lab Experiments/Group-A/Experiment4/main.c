#include<stdint.h>
#include<stdbool.h>
#include"inc/hw_memmap.h"
#include"inc/hw_types.h"
#include"driverlib/gpio.h"
#include"driverlib/pin_map.h"
#include"driverlib/sysctl.h"
#include"driverlib/uart.h"
#include"driverlib/debug.h"
#include"driverlib/adc.h"
#include"string.h"

char c1=48, c2=48;

int main(void){
	SysCtlClockSet(SYSCTL_SYSDIV_5 | SYSCTL_USE_PLL | SYSCTL_OSC_MAIN | SYSCTL_XTAL_16MHZ);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
	GPIOPinConfigure(GPIO_PA0_U0RX);
	GPIOPinConfigure(GPIO_PA1_U0TX);
	GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);
	UARTConfigSetExpClk(UART0_BASE, SysCtlClockGet(), 115200,
			(UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE | UART_CONFIG_PAR_NONE));
	UARTCharPut(UART0_BASE, 'E');
	UARTCharPut(UART0_BASE, 'n');
	UARTCharPut(UART0_BASE, 't');
	UARTCharPut(UART0_BASE, 'e');
	UARTCharPut(UART0_BASE, 'r');
	UARTCharPut(UART0_BASE, ' ');
	UARTCharPut(UART0_BASE, 'T');
	UARTCharPut(UART0_BASE, 'e');
	UARTCharPut(UART0_BASE, 'x');
	UARTCharPut(UART0_BASE, 't');
	UARTCharPut(UART0_BASE, ':');
	UARTCharPut(UART0_BASE, ' ');

	uint32_t ui32ADC0Value[4];
	volatile uint32_t ui32TempAvg;
	volatile uint32_t ui32TempValueC;
	volatile uint32_t ui32TempValueF;
	//SysCtlClockSet(SYSCTL_SYSDIV_5|SYSCTL_USE_PLL|SYSCTL_OSC_MAIN|SYSCTL_XTAL_16MHZ);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_ADC0);
	ADCSequenceConfigure(ADC0_BASE, 1, ADC_TRIGGER_PROCESSOR, 0);
	ADCSequenceStepConfigure(ADC0_BASE, 1, 0, ADC_CTL_TS);
	ADCSequenceStepConfigure(ADC0_BASE, 1, 1, ADC_CTL_TS);
	ADCSequenceStepConfigure(ADC0_BASE, 1, 2, ADC_CTL_TS);
	ADCSequenceStepConfigure(ADC0_BASE,1,3,ADC_CTL_TS|ADC_CTL_IE|ADC_CTL_END);
	ADCSequenceEnable(ADC0_BASE, 1);
	char i1, i2;

	//SysCtlClockSet(SYSCTL_SYSDIV_4|SYSCTL_USE_PLL|SYSCTL_XTAL_16MHZ|SYSCTL_OSC_MAIN);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
	GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3);

	while(1)
	{
		ADCIntClear(ADC0_BASE, 1);
		ADCProcessorTrigger(ADC0_BASE, 1);
		while(!ADCIntStatus(ADC0_BASE, 1, false))
		{
		}
		ADCSequenceDataGet(ADC0_BASE, 1, ui32ADC0Value);
		ui32TempAvg = (ui32ADC0Value[0] + ui32ADC0Value[1] + ui32ADC0Value[2] + ui32ADC0Value[3] + 2)/4;
		ui32TempValueC = (1475 - ((2475 * ui32TempAvg)) / 4096)/10;
		ui32TempValueF = ((ui32TempValueC * 9) + 160) / 5;

		i1 = ui32TempValueC/10 + 48;
		i2 = ui32TempValueC%10 + 48;


		UARTCharPut(UART0_BASE, 'C');
		UARTCharPut(UART0_BASE, 'u');
		UARTCharPut(UART0_BASE, 'r');
		UARTCharPut(UART0_BASE, 'r');
		UARTCharPut(UART0_BASE, 'e');
		UARTCharPut(UART0_BASE, 'n');
		UARTCharPut(UART0_BASE, 't');
		UARTCharPut(UART0_BASE, ' ');
		UARTCharPut(UART0_BASE, 'T');
		UARTCharPut(UART0_BASE, 'e');
		UARTCharPut(UART0_BASE, 'm');
		UARTCharPut(UART0_BASE, 'p');
		UARTCharPut(UART0_BASE, 'e');
		UARTCharPut(UART0_BASE, 'r');
		UARTCharPut(UART0_BASE, 'a');
		UARTCharPut(UART0_BASE, 't');
		UARTCharPut(UART0_BASE, 'u');
		UARTCharPut(UART0_BASE, 'r');
		UARTCharPut(UART0_BASE, 'e');
		UARTCharPut(UART0_BASE, ' ');
		UARTCharPut(UART0_BASE, i1);
		UARTCharPut(UART0_BASE, i2);
		UARTCharPut(UART0_BASE, ' ');
		UARTCharPut(UART0_BASE, 'C');
		UARTCharPut(UART0_BASE, ' ');
		//UARTCharPut(UART0_BASE, 15);

		char s_text[] = "Set Temperature : ";
		int s_len=strlen(s_text);
		int s=0;
		for(;s<s_len;s++)
		{
			UARTCharPut(UART0_BASE,s_text[s]);
		}
		UARTCharPut(UART0_BASE,c1);
		UARTCharPut(UART0_BASE,c2);
		UARTCharPut(UART0_BASE,' ');

		int current = (i1-48)*10 + i2;
		int set = (c1-48)*10 + c2;

		if(current < set )
		{
			GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3, 8);
		}
		else
		{
			GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3, 2);
		}

		SysCtlDelay(SysCtlClockGet()/3);

		if (UARTCharsAvail(UART0_BASE))
		{
			if(UARTCharGet(UART0_BASE) == 's')
			{

				char text[] = "Enter the temperatue : ";
				int len1=strlen(text);
				int i=0;
				for(; i<len1;i++)
				{
					UARTCharPut(UART0_BASE,text[i]);
				}
				c1 = UARTCharGet(UART0_BASE);
				UARTCharPut(UART0_BASE,c1);

				c2 = UARTCharGet(UART0_BASE);
				UARTCharPut(UART0_BASE,c2);

				char w_text[] = " Set temperature Updated to ";
				int j=0;
				int len2=strlen(w_text);
				for(; j<len2;j++)
				{
					UARTCharPut(UART0_BASE,w_text[j]);
				}
				UARTCharPut(UART0_BASE,c1);
				UARTCharPut(UART0_BASE,c2);
				UARTCharPut(UART0_BASE,' ');
				UARTCharPut(UART0_BASE,'C');
				UARTCharPut(UART0_BASE,' ');

			}
		}
	}
}
