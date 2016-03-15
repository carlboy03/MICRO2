#include <stdint.h>
#include <stdbool.h>
#include "inc/tm4c123gh6pm.h" //Definitions for interrupt and register assignment
#include "inc/hw_types.h"
#include "inc/hw_memmap.h"
#include "driverlib/sysctl.h"
#include "driverlib/interrupt.h" //Defines NVIC API
#include "driverlib/gpio.h"
#include "driverlib/timer.h" //Timer API
#include <stdlib.h>
#include <stdio.h>

#define buzzerPin GPIO_PIN_5
#define buzzerPinHigh GPIO_PIN_5
volatile uint8_t buzzerValue;
volatile int frequency;
uint32_t ui32Period;



#define buttonUpPin GPIO_PIN_2
#define buttonDownPin GPIO_PIN_3

void portAISR(void);



/******************************************************************
 * By: Alan López García and Carlos Fuentes Rosa 3/9/2016
 * References:
 * TI, Getting Started with the Tica TM4C132G LaunchPad Workhsop
 ******************************************************************/
int main(void) {


	SysCtlClockSet(SYSCTL_SYSDIV_4|SYSCTL_USE_PLL|SYSCTL_XTAL_16MHZ|SYSCTL_OSC_MAIN);  //Sets 40MHz clock

	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);
	GPIOPinTypeGPIOOutput(GPIO_PORTB_BASE, buzzerPin);
	buzzerValue = buzzerPinHigh;
	GPIOPinWrite(GPIO_PORTB_BASE, buzzerPin,buzzerValue);

	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
	GPIOPinTypeGPIOInput(GPIO_PORTA_BASE, buttonUpPin|buttonDownPin);
	GPIOIntDisable(GPIO_PORTA_BASE, buttonUpPin|buttonDownPin);
	GPIOIntClear(GPIO_PORTA_BASE, buttonUpPin|buttonDownPin);
	GPIOIntRegister(GPIO_PORTA_BASE, portAISR);
	GPIOIntTypeSet(GPIO_PORTA_BASE, buttonUpPin|buttonDownPin,GPIO_FALLING_EDGE);
	GPIOIntEnable(GPIO_PORTA_BASE, buttonUpPin|buttonDownPin);


	SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER0); //Using 32bit timer0A+timer0B
	TimerConfigure(TIMER0_BASE, TIMER_CFG_PERIODIC); //set clock in periodic mode

	frequency = 1024;
	ui32Period = (SysCtlClockGet() / frequency) / 2; //SysCtlClockGet()/desiredfrequency/dutyCycle
	TimerLoadSet(TIMER0_BASE, TIMER_A, ui32Period -1); // the load to the specific timer  -1 is used because the timer starts @ 0

	IntEnable(INT_TIMER0A);
	TimerIntEnable(TIMER0_BASE, TIMER_TIMA_TIMEOUT);
	IntMasterEnable();

	TimerEnable(TIMER0_BASE, TIMER_A);

	while(1){

	}

}

void frequencyPlus(){
	frequency = frequency+10;
	ui32Period = (SysCtlClockGet() / frequency) / 2; //SysCtlClockGet()/desiredfrequency/dutyCycle
	TimerLoadSet(TIMER0_BASE, TIMER_A, ui32Period -1); // the load to the specific timer  -1 is used because the timer starts @ 0
}

void frequencyLess(){
	frequency = frequency-10;
	ui32Period = (SysCtlClockGet() / frequency) / 2; //SysCtlClockGet()/desiredfrequency/dutyCycle
	TimerLoadSet(TIMER0_BASE, TIMER_A, ui32Period -1); // the load to the specific timer  -1 is used because the timer starts @ 0
}

void Timer0IntHandler(void){

	// Clear the timer interrupt

	TimerIntClear(TIMER0_BASE, TIMER_TIMA_TIMEOUT);
	buzzerValue = ~(buzzerValue);

	GPIOPinWrite(GPIO_PORTB_BASE, buzzerPin, buzzerValue);
}

void portAISR(void){
	GPIOIntDisable(GPIO_PORTA_BASE, buttonUpPin|buttonDownPin);
	if(GPIOIntStatus(GPIO_PORTA_BASE, false) & buttonUpPin){
		frequencyPlus();
		GPIOIntClear(GPIO_PORTA_BASE, buttonUpPin);
	}
	else if(GPIOIntStatus(GPIO_PORTA_BASE, false) & buttonDownPin){
		frequencyLess();
		GPIOIntClear(GPIO_PORTA_BASE, buttonDownPin);

	}
	GPIOIntEnable(GPIO_PORTA_BASE, buttonUpPin|buttonDownPin);

}
