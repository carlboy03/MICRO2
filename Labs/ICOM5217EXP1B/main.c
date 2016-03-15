#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_types.h"
#include "inc/hw_memmap.h"
#include "driverlib/sysctl.h"
#include "driverlib/gpio.h"
#include "driverlib/timer.h"

/******************************************************************
 * By: Alan López García and Carlos Fuentes Rosa
 * References:
 * TI, Getting Started with the Tica TM4C132G LaunchPad Workhsop
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
volatile int currentPhrase=0;




int main(void){
	SysCtlClockSet(SYSCTL_SYSDIV_4|SYSCTL_USE_PLL|SYSCTL_XTAL_16MHZ|SYSCTL_OSC_MAIN);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);



	GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, RS|E|RW);
	GPIOPinTypeGPIOOutput(GPIO_PORTA_BASE, bit4|bit5|bit6|bit7);
	GPIOPinTypeGPIOInput(GPIO_PORTA_BASE, buttonUpPin|buttonDownPin);

	FourBitInitialize();

	/*
	displayLine(hello, 11);
	while(1){}
	*/
	lineSwitcher(0);


	while(1){
		if (GPIOPinRead(GPIO_PORTA_BASE, buttonUpPin) == 0) {
			currentPhrasePlus();
		}
		if (GPIOPinRead(GPIO_PORTA_BASE, buttonDownPin) == 0) {
			currentPhraseLess();
		}
	}


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

void lineSwitcher(int currentCase){
	switch(currentCase) {

	case 0  :
		displayLine(line0,lineLengthArray[currentCase] );
		break;

	case 1  :
	{
		displayLine(line1,lineLengthArray[currentCase] );
	}
	break;
	case 2  :
	{
		displayLine(line2,lineLengthArray[currentCase] );
	}
	break;
	case 3  :
	{
		displayLine(line3,lineLengthArray[currentCase] );
	}
	break;
	case 4  :
	{
		displayLine(line4,lineLengthArray[currentCase] );
	}
	break;
	case 5  :
	{
		displayLine(line5,lineLengthArray[currentCase] );
	}
	break;
	case 6  :
	{
		displayLine(line6,lineLengthArray[currentCase] );
	}
	break;
	case 7  :
	{
		displayLine(line7,lineLengthArray[currentCase] );
	}
	break;
	case 8  :
	{
		displayLine(line8,lineLengthArray[currentCase] );
	}
	break;
	case 9  :
	{
		displayLine(line9,lineLengthArray[currentCase] );
	}
	break;
	case 10  :
	{
		displayLine(line10,lineLengthArray[currentCase] );
	}
	break;
	case 11  :
	{
		displayLine(line11,lineLengthArray[currentCase] );
	}
	break;
	case 12  :
	{
		displayLine(line12,lineLengthArray[currentCase] );
	}
	break;
	case 13  :
	{
		displayLine(line13,lineLengthArray[currentCase] );
	}
	break;
	case 14  :
	{
		displayLine(line14,lineLengthArray[currentCase] );
	}
	break;
	case 15  :
	{
		displayLine(line15,lineLengthArray[currentCase] );
	}
	break;
	}

}

void currentPhrasePlus(){

	if (currentPhrase<16){
		currentPhrase++;
	}
	else{
		currentPhrase=0;
	}
	lineSwitcher(currentPhrase);
}

void currentPhraseLess(){

	if (currentPhrase>-1){
		currentPhrase--;
	}
	else{
		currentPhrase=15;
	}
	lineSwitcher(currentPhrase);
}


