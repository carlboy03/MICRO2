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

uint32_t restartGame;
uint32_t gameOver;
uint32_t timerValue;
uint32_t gameStatus;

void portAISR(void);



/******************************************************************
 * By: Alan López García and Carlos Fuentes Rosa 3/9/2016
 * References:
 * TI, Getting Started with the Tica TM4C132G LaunchPad Workhsop
 ******************************************************************/
int main(void) {

	timerValue=0;
	gameOver=0;
	restartGame=1;
	gameStatus=0;
	SysCtlClockSet(SYSCTL_SYSDIV_4|SYSCTL_USE_PLL|SYSCTL_XTAL_16MHZ|SYSCTL_OSC_MAIN);  //Sets 40MHz clock

	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);

	GPIOPinTypeGPIOOutput(GPIO_PORTB_BASE, buzzerPin);
	buzzerValue = buzzerPinHigh;
	GPIOPinWrite(GPIO_PORTB_BASE, buzzerPin,buzzerValue);

	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
	GPIOPinTypeGPIOInput(GPIO_PORTA_BASE, buttonUpPin|buttonDownPin);
	//GPIOIntDisable(GPIO_PORTA_BASE, buttonUpPin|buttonDownPin);
	//GPIOIntClear(GPIO_PORTA_BASE, buttonUpPin|buttonDownPin);
	//GPIOIntRegister(GPIO_PORTA_BASE, portAISR);
	//GPIOIntTypeSet(GPIO_PORTA_BASE, buttonUpPin|buttonDownPin,GPIO_RISING_EDGE);
	//GPIOIntEnable(GPIO_PORTA_BASE, buttonUpPin|buttonDownPin);
	GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, RS|E|RW);
	GPIOPinTypeGPIOOutput(GPIO_PORTA_BASE, bit4|bit5|bit6|bit7);

	SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER0); //Using 32bit timer0A+timer0B
	TimerConfigure(TIMER0_BASE, TIMER_CFG_PERIODIC_UP); //set clock in periodic mode

	//frequency = 0xFFFF;
	//ui32Period = (SysCtlClockGet() / frequency) / 20; //SysCtlClockGet()/desiredfrequency/dutyCycle
	TimerLoadSet(TIMER0_BASE, TIMER_A,0xFFFF); // the load to the specific timer  -1 is used because the timer starts @ 0

	//IntEnable(INT_TIMER0A);
	TimerIntEnable(TIMER0_BASE, TIMER_TIMA_TIMEOUT);
	IntMasterEnable();

	TimerEnable(TIMER0_BASE, TIMER_A);
	FourBitInitialize();
	restartMessage();
	while(1){

		if(GPIOPinRead(GPIO_PORTA_BASE, buttonUpPin)==0){
			gameStatus1();
			printRandom();
		}
		else if(GPIOPinRead(GPIO_PORTA_BASE, buttonDownPin)==0 && GPIOPinRead(GPIO_PORTA_BASE, buttonUpPin)==0){
			send4BitCommand(1);
			sendCharacter(':');
			sendCharacter(')');
			sendCharacter(' ');
			sendCharacter('W');
			sendCharacter('i');
			sendCharacter('n');
			sendCharacter('!');
			millisDelay(25);
			restartMessage();
			gameStatus=0;


		}
	}

}
void portAISR(void){
	GPIOIntDisable(GPIO_PORTA_BASE, buttonUpPin|buttonDownPin);
	if(GPIOIntStatus(GPIO_PORTA_BASE, true) & buttonUpPin && GPIOIntStatus(GPIO_PORTA_BASE, true) & buttonDownPin){
		send4BitCommand(1);
		sendCharacter(':');
		sendCharacter(')');
		restartGame=1;
	}

	GPIOIntEnable(GPIO_PORTA_BASE, buttonUpPin|buttonDownPin);

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

void printRandom(){
	send4BitCommand(1);
	timerValue = TimerValueGet(TIMER0_BASE, TIMER_A);

	timerValue= timerValue& 0x00FF;
	timerValue=timerValue%10;

	switch(timerValue){

	case 0: {
		sendCharacter('0');
	}
	break;
	case 1: {
		sendCharacter('1');
	}
	break;
	case 2: {
		sendCharacter('2');
	}
	break;
	case 3: {
		sendCharacter('3');
	}
	break;
	case 4: {
		sendCharacter('4');
	}
	break;
	case 5: {
		sendCharacter('5');
	}
	break;
	case 6: {
		sendCharacter('6');
	}
	break;
	case 7: {
		sendCharacter('7');
	}
	break;
	case 8: {
		sendCharacter('8');
	}
	break;
	case 9: {
		sendCharacter('9');
	}
	break;

	}
	return;
}

void restartMessage(){
	send4BitCommand(1);
	sendCharacter('G');
	sendCharacter('u');
	sendCharacter('e');
	sendCharacter('s');
	sendCharacter('s');
	sendCharacter(' ');
	sendCharacter('#');
	return;


}
void gameOverMessage(){
	send4BitCommand(1);
	sendCharacter('G');
	sendCharacter('a');
	sendCharacter('m');
	sendCharacter('e');
	sendCharacter(' ');
	sendCharacter('O');
	sendCharacter('v');
	sendCharacter('e');
	sendCharacter('r');
return;
}

void gameStatus1(){
	//if 0<=gameStatus<3 it is OKay to go
	//if 3 game over
	if(gameStatus == 3){
		gameOverMessage();
		millisDelay(50);
		restartMessage();
		millisDelay(50);
		gameStatus=0;
	}
	if (gameStatus>=0 && gameStatus<3){
		gameStatus++;

	}
return;
}
