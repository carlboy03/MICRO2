#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_memmap.h"
#include "inc/hw_ssi.h"
#include "inc/hw_types.h"
#include "driverlib/ssi.h"
#include "driverlib/gpio.h"
#include "driverlib/pin_map.h"
#include "driverlib/sysctl.h"

#define NUM_SSI_DATA 8
const uint8_t pui8DataTx[NUM_SSI_DATA] =
{0x1, 0x2, 0x3, 0x4, 0x5, 0x6, 0x7, 0x8};
uint8_t RS = GPIO_PIN_1;
uint8_t E = GPIO_PIN_2;
uint8_t RW = GPIO_PIN_3;

uint8_t bit0 = GPIO_PIN_0;
uint8_t bit1 = GPIO_PIN_1;
uint8_t bit2 = GPIO_PIN_2;
uint8_t bit3 = GPIO_PIN_3;

uint8_t RSHigh = 2;
uint8_t EHigh = 4;
uint8_t RWHigh = 8;

uint8_t bit0High = 1;
uint8_t bit1High = 2;
uint8_t bit2High = 4;
uint8_t bit3High = 8;
int i,j;
uint8_t temp;
char string[16];


// Bit-wise reverses a number.
uint8_t
Reverse(uint8_t ui8Number)
{
	uint8_t ui8Index;
	uint8_t ui8ReversedNumber = 0;
	for(ui8Index=0; ui8Index<8; ui8Index++)
	{
		ui8ReversedNumber = ui8ReversedNumber << 1;
		ui8ReversedNumber |= ((1 << ui8Index) & ui8Number) >> ui8Index;
	}
	return ui8ReversedNumber;
}
uint32_t dataReceived;
uint32_t *dataReceived1 = &dataReceived;
int main(void)
{
	uint32_t ui32Index;
	uint32_t ui32Data;
	dataReceived=0;

	SysCtlClockSet(SYSCTL_SYSDIV_4 | SYSCTL_USE_PLL | SYSCTL_OSC_MAIN | SYSCTL_XTAL_16MHZ);
//Setup SPI
	SysCtlPeripheralEnable(SYSCTL_PERIPH_SSI0);
		SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);


		GPIOPinConfigure(GPIO_PA2_SSI0CLK);
		GPIOPinConfigure(GPIO_PA3_SSI0FSS);
		GPIOPinConfigure(GPIO_PA5_SSI0TX);
		GPIOPinConfigure(GPIO_PA4_SSI0RX);

		GPIOPinTypeSSI(GPIO_PORTA_BASE,GPIO_PIN_5|GPIO_PIN_4|GPIO_PIN_3|GPIO_PIN_2);

		SSIConfigSetExpClk(SSI0_BASE, SysCtlClockGet(), SSI_FRF_MOTO_MODE_0, SSI_MODE_MASTER, 1000, 8);
		SSIEnable(SSI0_BASE);
		//Setup LCD
					//SysCtlClockSet(SYSCTL_SYSDIV_4|SYSCTL_USE_PLL|SYSCTL_XTAL_16MHZ|SYSCTL_OSC_MAIN);
					SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
					SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOD);

					GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, RS|E|RW);
					GPIOPinTypeGPIOOutput(GPIO_PORTD_BASE, bit0|bit1|bit2|bit3);



	ui32Data = 'c';
FourBitInitialize();
sendCharacter('c');
sendCharacter('c');
sendCharacter('c');
sendCharacter('c');
sendCharacter('c');
sendCharacter('c');
sendCharacter('c');

	//SSIDataPut(SSI0_BASE, dataReceived);

	//		for(ui32Index = 0; ui32Index < NUM_SSI_DATA; ui32Index++)
	//		{
	//			//ui32Data = (Reverse(pui8DataTx[ui32Index]) << 8) + (1 << ui32Index);
	//			//SSIDataPut(SSI0_BASE, ui32Data);
	//
	//
	//			while(SSIBusy(SSI0_BASE))
	//			{
	//			}
	//		}
	while(1)
	{
		SSIDataPut(SSI0_BASE, ui32Data);
		SSIDataGet(SSI0_BASE, dataReceived1);
					while(SSIBusy(SSI0_BASE))
						{
						}
	}

}

void send6BitCommand(uint8_t command){
	GPIOPinWrite(GPIO_PORTF_BASE, E, EHigh);
	GPIOPinWrite(GPIO_PORTF_BASE, RS|RW ,command );
	GPIOPinWrite(GPIO_PORTD_BASE,  bit0|bit1|bit2|bit3, command>>4);
	GPIOPinWrite(GPIO_PORTF_BASE, E, ~(EHigh));
	millisDelay(2);
}

void send4BitCommand(uint8_t command){
	GPIOPinWrite(GPIO_PORTF_BASE, E, EHigh);
	GPIOPinWrite(GPIO_PORTD_BASE, bit0|bit1|bit2|bit3, command>>4);
	GPIOPinWrite(GPIO_PORTF_BASE, E, ~(EHigh));
	millisDelay(1);
	GPIOPinWrite(GPIO_PORTF_BASE, E, EHigh);
	GPIOPinWrite(GPIO_PORTD_BASE, bit3|bit2|bit1|bit0, command);
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


