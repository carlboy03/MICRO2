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
 * By: Alan López García and Carlos Fuentes Rosa 3/2/2016
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

#define photosensorTop GPIO_PIN_2
#define photosensorBottom GPIO_PIN_3


volatile char buffer [16];
volatile int  sizeOfInteger;
void portAISR(void);
void portCISR(void);
void lineSwitcher();


volatile uint8_t currentState;
volatile int globalCounter;

static char line0[2] = "La";//2
static char line1[11] = "computadora";//11
static char line2[2] = "es";//2
static char line3[3] = "tan";//3
static char line4[11] = "inteligente";//11
static char line5[4] = "como";//4
static char line6[2] = "tu";//2
static char line7[2] = "lo";//2
static char line8[5] = "seas.";//5
static char line9[10] = "-El procer";//10
static char line10[12] = "Danilo Rojas";//12
static char line11[2] = "Un";//2
static char line12[2] = "23";//2
static char line13[10] = "de febrero";//10
static char line14[7] = "de 2016";//7
static char line15[8] = "Linea 16";//8
static char hello[11] = "hello world";

volatile int lineLengthArray[16] = {2,11,2,3,11,4,2,2,5,10,12,2,2,10,7,8};


int main(void){
    SysCtlClockSet(SYSCTL_SYSDIV_4|SYSCTL_USE_PLL|SYSCTL_XTAL_16MHZ|SYSCTL_OSC_MAIN);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
	//SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOC);




	GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, RS|E|RW);
	GPIOPinTypeGPIOOutput(GPIO_PORTA_BASE, bit4|bit5|bit6|bit7);
	GPIOPinTypeGPIOInput(GPIO_PORTA_BASE, photosensorTop | photosensorBottom);
	//GPIOPinTypeGPIOInput(GPIO_PORTC_BASE, buttonDebouncePin);



	// Interrupt setup 1
	GPIOIntDisable(GPIO_PORTA_BASE, photosensorTop | photosensorBottom);
	GPIOIntClear(GPIO_PORTA_BASE, photosensorTop | photosensorBottom);
	GPIOIntRegister(GPIO_PORTA_BASE, portAISR);
	GPIOIntTypeSet(GPIO_PORTA_BASE, photosensorTop | photosensorBottom,GPIO_BOTH_EDGES); //both edges pq detecto 1 y 0 dependiendo el color
	GPIOIntEnable(GPIO_PORTA_BASE,photosensorTop | photosensorBottom);


	FourBitInitialize();

	sprintf(buffer, "%d", globalCounter);
	//updateSizeOfInteger();
	//displayLine(buffer,sizeOfInteger); //cast char

	currentState= (GPIOIntStatus(GPIO_PORTA_BASE, false)& (photosensorTop | photosensorBottom));
	currentState = currentState<<2;
	currentState = 0x0F & currentState;
	globalCounter=-1;


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

	if (globalCounter<16){
		globalCounter++;
	}
	else{
		globalCounter=0;
	}
	lineSwitcher();
}

void globalCounterLess(){
	if (globalCounter>-1){
		globalCounter--;
	}
	else{
		globalCounter=15;
	}
	lineSwitcher();
}

void portAISR(void){
	GPIOIntDisable(GPIO_PORTA_BASE, photosensorTop | photosensorBottom);
	uint8_t tempVariable= (GPIOPinRead(GPIO_PORTA_BASE,(photosensorTop | photosensorBottom)));
	//uint8_t tempVariableBottom= (GPIOIntStatus(GPIO_PORTA_BASE, false)& (photosensorBottom));
	//uint8_t tempVariableTop= (GPIOIntStatus(GPIO_PORTA_BASE, false)& (photosensorTop));
	//tempVariable= (photosensorTop | photosensorBottom) & tempVariable;
	//uint32_t tempVariable2= GPIOIntStatus(GPIO_PORTA_BASE, false);
	//currentState =  currentState << 2 ;
	currentState = currentState<<2|tempVariable>>2;
	currentState = 0x0F & currentState;
	tableLookup();
	//currentState = GPIOIntStatus(GPIO_PORTA_BASE, false) & photosensorTop | GPIOIntStatus(GPIO_PORTA_BASE, false) & photosensorBottom;
	//printf("tempVariableTop: %d \n", tempVariableTop);
	//printf("tempVariableBottom: %d \n", tempVariableBottom);
	GPIOIntClear(GPIO_PORTA_BASE, photosensorTop | photosensorBottom);
	GPIOIntEnable(GPIO_PORTA_BASE, photosensorTop | photosensorBottom);


}

void portCISR(void){


}

void lineSwitcher(){
	switch(globalCounter) {

	case 0  :
		displayLine(line0,lineLengthArray[globalCounter] );
		break;

	case 1  :
	{
		displayLine(line1,lineLengthArray[globalCounter] );
	}
	break;
	case 2  :
	{
		displayLine(line2,lineLengthArray[globalCounter] );
	}
	break;
	case 3  :
	{
		displayLine(line3,lineLengthArray[globalCounter] );
	}
	break;
	case 4  :
	{
		displayLine(line4,lineLengthArray[globalCounter] );
	}
	break;
	case 5  :
	{
		displayLine(line5,lineLengthArray[globalCounter] );
	}
	break;
	case 6  :
	{
		displayLine(line6,lineLengthArray[globalCounter] );
	}
	break;
	case 7  :
	{
		displayLine(line7,lineLengthArray[globalCounter] );
	}
	break;
	case 8  :
	{
		displayLine(line8,lineLengthArray[globalCounter] );
	}
	break;
	case 9  :
	{
		displayLine(line9,lineLengthArray[globalCounter] );
	}
	break;
	case 10  :
	{
		displayLine(line10,lineLengthArray[globalCounter] );
	}
	break;
	case 11  :
	{
		displayLine(line11,lineLengthArray[globalCounter] );
	}
	break;
	case 12  :
	{
		displayLine(line12,lineLengthArray[globalCounter] );
	}
	break;
	case 13  :
	{
		displayLine(line13,lineLengthArray[globalCounter] );
	}
	break;
	case 14  :
	{
		displayLine(line14,lineLengthArray[globalCounter] );
	}
	break;
	case 15  :
	{
		displayLine(line15,lineLengthArray[globalCounter] );
	}
	break;
	}

}

void tableLookup(){
	switch(currentState) {

	case 0  :{}
	break;
	case 1  : {globalCounterPlus();}
	break;
	case 2  :{globalCounterLess();}
	break;
	case 3  :{}
	break;
	case 4  :{globalCounterLess();}
	break;
	case 5  :{}
	break;
	case 6  :{}
	break;
	case 7  :{globalCounterPlus();}
	break;
	case 8  :{globalCounterPlus();}
	break;
	case 9  :{}
	break;
	case 10  :{}
	break;
	case 11  :{globalCounterLess();}
	break;
	case 12  :{}
	break;
	case 13  :{globalCounterLess();}
	break;
	case 14  :{globalCounterPlus();}
	break;
	case 15  :{}
	break;
	}

}
