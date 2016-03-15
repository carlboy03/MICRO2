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

/******************************************************************
 * By: Alan López García and Carlos Fuentes Rosa 3/14/2016
 * References:
 * TI, Getting Started with the Tica TM4C132G LaunchPad Workhsop
 * https://e2e.ti.com/support/microcontrollers/tiva_arm/f/908/t/332605
 ******************************************************************/
uint8_t RS = GPIO_PIN_1;
uint8_t E = GPIO_PIN_2;
uint8_t RW = GPIO_PIN_3;

uint8_t bit4 = GPIO_PIN_4;
uint8_t bit5 = GPIO_PIN_5;
uint8_t bit6 = GPIO_PIN_6;
uint8_t bit7 = GPIO_PIN_7;

uint8_t RSHigh = 2;
uint8_t EHigh = 4;
uint8_t RWHigh = 8;

uint8_t bit4High = 16;
uint8_t bit5High = 32;
uint8_t bit6High = 64;
uint8_t bit7High = 128;





uint8_t buttonUpPin = GPIO_PIN_3;
uint8_t buttonDownPin = GPIO_PIN_2;
uint8_t buttonDebouncePin = GPIO_PIN_6;


volatile char buffer [16];
volatile int  sizeOfInteger;
void portAISR(void);
void portCISR(void);
void portDISR(void);



volatile int globalCounter=-1;




int main(void){
	SysCtlClockSet(SYSCTL_SYSDIV_4|SYSCTL_USE_PLL|SYSCTL_XTAL_16MHZ|SYSCTL_OSC_MAIN);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOC);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOD);




	GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, RS|E|RW);
	GPIOPinTypeGPIOOutput(GPIO_PORTA_BASE, bit4|bit5|bit6|bit7);
	GPIOPinTypeGPIOInput(GPIO_PORTA_BASE, buttonUpPin|buttonDownPin);
	GPIOPinTypeGPIOInput(GPIO_PORTA_BASE, buttonUpPin|buttonDownPin);//Sensors are in portD2-3
	GPIOPinTypeGPIOInput(GPIO_PORTC_BASE, buttonDebouncePin);



	// Interrupt setup 1
	GPIOIntDisable(GPIO_PORTA_BASE, buttonUpPin|buttonDownPin);
	GPIOIntClear(GPIO_PORTA_BASE, buttonUpPin|buttonDownPin);
	GPIOIntRegister(GPIO_PORTA_BASE, portAISR);
	GPIOIntTypeSet(GPIO_PORTA_BASE, buttonUpPin|buttonDownPin,GPIO_FALLING_EDGE);
	GPIOIntEnable(GPIO_PORTA_BASE, buttonUpPin|buttonDownPin);


	// Interrupt setup 2
	GPIOIntDisable(GPIO_PORTC_BASE, buttonDebouncePin);
	GPIOIntClear(GPIO_PORTC_BASE, buttonDebouncePin);
	GPIOIntRegister(GPIO_PORTC_BASE, portCISR);
	GPIOIntTypeSet(GPIO_PORTC_BASE, buttonDebouncePin,GPIO_FALLING_EDGE);
	GPIOIntEnable(GPIO_PORTC_BASE, buttonDebouncePin);


	// Interrupt setup 3 PD2/PD3
	GPIOIntDisable(GPIO_PORTD_BASE, buttonUpPin|buttonDownPin);
	GPIOIntClear(GPIO_PORTD_BASE, buttonUpPin|buttonDownPin);
	GPIOIntRegister(GPIO_PORTD_BASE, portDISR);
	GPIOIntTypeSet(GPIO_PORTD_BASE, buttonUpPin|buttonDownPin,GPIO_FALLING_EDGE);
	GPIOIntEnable(GPIO_PORTD_BASE, buttonUpPin|buttonDownPin);

	FourBitInitialize();

	sprintf(buffer, "%d", globalCounter);
	updateSizeOfInteger();
	displayLine(buffer,sizeOfInteger); //cast char


	while(1); //busyWait




}

void send6BitCommand(uint8_t command){
	GPIOPinWrite(GPIO_PORTF_BASE, E, EHigh);
	GPIOPinWrite(GPIO_PORTF_BASE, RS|RW ,command );
	GPIOPinWrite(GPIO_PORTA_BASE, bit4|bit5|bit6|bit7, command);
	GPIOPinWrite(GPIO_PORTF_BASE, E, ~(EHigh));
	millisDelay(2);
}

void send4BitCommand(uint8_t command){
	GPIOPinWrite(GPIO_PORTF_BASE, E, EHigh);
	GPIOPinWrite(GPIO_PORTA_BASE, bit4|bit5|bit6|bit7, command);
	GPIOPinWrite(GPIO_PORTF_BASE, E, ~(EHigh));
	millisDelay(1);
	GPIOPinWrite(GPIO_PORTF_BASE, E, EHigh);
	GPIOPinWrite(GPIO_PORTA_BASE, bit4|bit5|bit6|bit7, command << 4);
	GPIOPinWrite(GPIO_PORTF_BASE, E, ~(EHigh));
	millisDelay(1);
}


void FourBitInitialize(){
	millisDelay(18);
	uint8_t tempCommand = 48;
	send6BitCommand(tempCommand);
	millisDelay(7);
	send6BitCommand(tempCommand);
	// delay needed < 2
	send6BitCommand(tempCommand);
	tempCommand = 32;
	send6BitCommand(tempCommand);
	//Sigue siendo 32 porque N =0 y F = 0
	send4BitCommand(tempCommand);
	tempCommand = 8;
	send4BitCommand(tempCommand);
	tempCommand = 1;
	send4BitCommand(tempCommand);
	tempCommand = 6;
	send4BitCommand(tempCommand);
	tempCommand = 13;
	send4BitCommand(tempCommand);


}

void sendCharacter(char character){
	GPIOPinWrite(GPIO_PORTF_BASE, RS, RSHigh);
	send4BitCommand(character);
	GPIOPinWrite(GPIO_PORTF_BASE, RS, ~(RSHigh));
}

void millisDelay(int millis){
	SysCtlDelay(millis*333333);
}


void displayLine(char line[], int length){
	//borrar Display
	send4BitCommand(1);
	int i;
	for(i=0; i<length; i++){
		sendCharacter(line[i]);
	}
}


void globalCounterPlus(){

	globalCounter++;
	sprintf(buffer, "%d", globalCounter);
	updateSizeOfInteger();
	displayLine(buffer,sizeOfInteger);
}

void globalCounterLess(){

	globalCounter--;
	sprintf(buffer, "%d", globalCounter);
	updateSizeOfInteger();
	displayLine(buffer,sizeOfInteger);
}

void portAISR(void){
	GPIOIntDisable(GPIO_PORTA_BASE, buttonUpPin|buttonDownPin);
	millisDelay(20);
	GPIOIntEnable(GPIO_PORTA_BASE, buttonUpPin|buttonDownPin);
	if(GPIOIntStatus(GPIO_PORTA_BASE, false) & buttonUpPin){
		//displayLine("buttonUpPin",11);
		globalCounterPlus();
		GPIOIntClear(GPIO_PORTA_BASE, buttonUpPin);
	}
	else if(GPIOIntStatus(GPIO_PORTA_BASE, false) & buttonDownPin){
			//displayLine("buttonDownPin",13);
		globalCounterLess();
		GPIOIntClear(GPIO_PORTA_BASE, buttonDownPin);

	}
}

void portCISR(void){
	if(GPIOIntStatus(GPIO_PORTC_BASE, false) & buttonDebouncePin){
		//displayLine("buttonUpPin",11);
		globalCounterPlus();
		GPIOIntClear(GPIO_PORTC_BASE, buttonDebouncePin);
	}

}

void updateSizeOfInteger(){
 if (globalCounter <= -100 && globalCounter >-10000) sizeOfInteger = 4;
 if (globalCounter <= -10 && globalCounter >-100) sizeOfInteger = 3;
 if (globalCounter <= -1 && globalCounter >-10) sizeOfInteger = 2;
 if (globalCounter <= 9 && globalCounter >-1) sizeOfInteger = 1;
 if (globalCounter <= 99 && globalCounter >9) sizeOfInteger = 2;
 if (globalCounter <= 999 && globalCounter >99) sizeOfInteger = 3;

}


void portDISR(void){

}
