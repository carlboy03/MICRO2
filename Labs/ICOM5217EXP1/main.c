#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_types.h"
#include "inc/hw_memmap.h"
#include "driverlib/sysctl.h"
#include "driverlib/gpio.h"

/******************************************************************
 * By: Alan López García and Carlos Fuentes Rosa
 * References:
 * TI, Getting Started with the Tica TM4C132G LaunchPad Workhsop
 ******************************************************************/
int main(void)
{
	uint8_t ledValue = 4;
	uint8_t ledPin = GPIO_PIN_2;
	uint8_t buttonPin = GPIO_PIN_4;
	SysCtlClockSet(SYSCTL_SYSDIV_4|SYSCTL_USE_PLL|SYSCTL_XTAL_16MHZ|SYSCTL_OSC_MAIN);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
	GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_2);
	GPIOPinTypeGPIOInput(GPIO_PORTF_BASE, buttonPin);
	GPIOPinWrite(GPIO_PORTF_BASE, ledPin, 4);
	while(1)
	{

		/*
		// Delay for a bit
		SysCtlDelay(2000000);
		*/
		// Cycle through Red, Green and Blue LEDs
		if (GPIOPinRead(GPIO_PORTF_BASE, buttonPin) == 0) {
			ledValue = ~(ledValue);
			GPIOPinWrite(GPIO_PORTF_BASE, ledPin, ledValue);
			SysCtlDelay(2000000);
		}



	}
}
