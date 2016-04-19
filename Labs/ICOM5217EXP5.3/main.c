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

	SysCtlPeripheralEnable(SYSCTL_PERIPH_SSI0);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);

	GPIOPinConfigure(GPIO_PD0_SSI.CLK);
	GPIOPinConfigure(GPIO_PD1_SSI3FSS);
	GPIOPinConfigure(GPIO_PD2_SSI3RX);
	GPIOPinConfigure(GPIO_PD3_SSI3TX);
	GPIOPinTypeSSI(GPIO_PORTD_BASE,GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_3|GPIO_PIN_2);

	SSIConfigSetExpClk(SSI3_BASE, SysCtlClockGet(), SSI_FRF_MOTO_MODE_0, SSI_MODE_MASTER, 1000, 8);
	SSIEnable(SSI3_BASE);


	ui32Data = 'c';


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
		SSIDataPut(SSI3_BASE, ui32Data);
		SSIDataGet(SSI3_BASE, dataReceived1);
					while(SSIBusy(SSI3_BASE))
						{
						}
	}

}

