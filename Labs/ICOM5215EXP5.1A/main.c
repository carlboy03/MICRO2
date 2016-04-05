#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_ints.h"
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/gpio.h"
#include "driverlib/interrupt.h"
#include "driverlib/pin_map.h"
#include "driverlib/sysctl.h"
#include "driverlib/uart.h"


/******************************************************************
 * By: Alan López García and Carlos Fuentes Rosa 4/5/2016
 * References:
 * TI, Getting Started with the Tica TM4C132G LaunchPad Workhsop
 * https://e2e.ti.com/support/microcontrollers/tiva_arm/f/908/t/332605
 ******************************************************************/
void UARTIntHandler(void)
{
	uint32_t ui32Status;

	ui32Status = UARTIntStatus(UART0_BASE, true); //get interrupt status

	UARTIntClear(UART0_BASE, ui32Status); //clear the asserted interrupts

	while(UARTCharsAvail(UART0_BASE)) //loop while there are chars
	{
		UARTCharPutNonBlocking(UART0_BASE, UARTCharGetNonBlocking(UART0_BASE)); //echo character
		GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_2, GPIO_PIN_2); //blink LED
		SysCtlDelay(SysCtlClockGet() / (1000 * 3)); //delay ~1 msec
		GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_2, 0); //turn off LED
	}
}

int main(void) {

	SysCtlClockSet(SYSCTL_SYSDIV_4 | SYSCTL_USE_PLL | SYSCTL_OSC_MAIN | SYSCTL_XTAL_16MHZ);

	SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);

	GPIOPinConfigure(GPIO_PA0_U0RX);
	GPIOPinConfigure(GPIO_PA1_U0TX);
	GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);

	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF); //enable GPIO port for LED
	GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_2); //enable pin for LED PF2

	UARTConfigSetExpClk(UART0_BASE, SysCtlClockGet(), 9600,
			(UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE | UART_CONFIG_PAR_NONE));

	IntMasterEnable(); //enable processor interrupts
	IntEnable(INT_UART0); //enable the UART interrupt
	UARTIntEnable(UART0_BASE, UART_INT_RX | UART_INT_RT); //only enable RX and TX interrupts
	UARTCharPut(UART0_BASE, '\r');
	UARTCharPut(UART0_BASE, '\n');
	UARTCharPut(UART0_BASE, 'H');
	UARTCharPut(UART0_BASE, 'e');
	UARTCharPut(UART0_BASE, 'l');
	UARTCharPut(UART0_BASE, 'l');
	UARTCharPut(UART0_BASE, 'o');
	UARTCharPut(UART0_BASE, 'W');
	UARTCharPut(UART0_BASE, 'o');
	UARTCharPut(UART0_BASE, 'r');
	UARTCharPut(UART0_BASE, 'l');
	UARTCharPut(UART0_BASE, 'd');
	UARTCharPut(UART0_BASE, '!');
	while (1) //let interrupt handler do the UART echo function
	{
		//    	if (UARTCharsAvail(UART0_BASE)) UARTCharPut(UART0_BASE, UARTCharGet(UART0_BASE));
	}

}
