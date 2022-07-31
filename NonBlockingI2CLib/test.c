/*
 * NonBlockingI2CLib.c
 *
 * Created: 7/19/2022 1:45:52 AM
 * Author : jack2
 */ 

#define F_CPU 16000000
#include <avr/io.h>
#include <stdlib.h>
#include <util/delay.h>
#include <stdio.h>

#include "I2CInstruction.h"
#include "I2CDriver.h"
#include "Usart.h"

void VL6180XInit(I2CBuffer_pT buf)
{	
	_delay_ms(50);
	I2CSetCurBuf(buf);
	
	uint8_t whoAmIRegAdd[2] = {0x00, 0x00};
	uint8_t * whomi = calloc(1, sizeof(uint8_t));
	
	while (*whomi != 0xb4)
	{
		I2CBufferAddInstruction(buf, 0x29, 0, &(whoAmIRegAdd[0]), 2);		// Tell the sensor we want to read an ID reg
		I2CInstruction_pT ipt1 = I2CBufferAddInstruction(buf, 0x29, 1, whomi, 1);	// Read the ID reg
		while (I2CBufferContains(buf, ipt1))					// Wait until the last two instructions finish or get stuck forever
		{
			I2CTask();
			usartTask();
		}
// 		char out[10];
// 		sprintf(out, "%d %d %d\t", *whomi, I2CBufferGetCurrentSize(buf), I2CBufferContains(buf, ipt1));
// 		addStringToUsartWriteBuffer(out);
		
	}
	PORTC |= (1<<PORTC6);
	//free(whomi);
	
	uint8_t VL6180XRequiredInitData[30][3] =
	{
		{0x02, 0x07, 0x01},
		{0x02, 0x08, 0x01},
		{0x00, 0x96, 0x00},
		{0x00, 0x97, 0xfd},
		{0x00, 0xe3, 0x00},
		{0x00, 0xe4, 0x04},
		{0x00, 0xe5, 0x02},
		{0x00, 0xe6, 0x01},
		{0x00, 0xe7, 0x03},
		{0x00, 0xf5, 0x02},
		{0x00, 0xd9, 0x05},
		{0x00, 0xdb, 0xce},
		{0x00, 0xdc, 0x03},
		{0x00, 0xdd, 0xf8},
		{0x00, 0x9f, 0x00},
		{0x00, 0xa3, 0x3c},
		{0x00, 0xb7, 0x00},
		{0x00, 0xbb, 0x3c},
		{0x00, 0xb2, 0x09},
		{0x00, 0xca, 0x09},
		{0x01, 0x98, 0x01},
		{0x01, 0xb0, 0x17},
		{0x01, 0xad, 0x00},
		{0x00, 0xff, 0x05},
		{0x01, 0x00, 0x05},
		{0x01, 0x99, 0x05},
		{0x01, 0xa6, 0x1b},
		{0x01, 0xac, 0x3e},
		{0x01, 0xa7, 0x1f},
		{0x00, 0x30, 0x00}
	};
	
	// For each of the private registers
	for(int i = 0; i < 30; i++)
	{
		// Write the correct values
		while(!I2CBufferAddInstruction(buf, 0x29, 0, &(VL6180XRequiredInitData[i][0]), 3));
	}
	
	static uint8_t VL6180XCustomInitData[12][3] =
	{
		{0x00, 0x11, 0x10},		// Turns on GPIO1 as interrupting when data is available
		{0x01, 0x0a, 0x30},		// Sets how long it averages range measurements
		{0x00, 0x3f, 0x46},		// Sets the light and dark gain
		{0x00, 0x31, 0x64},		// Sets how often temperature measurements are done (100)
		{0x00, 0x40, 0x63},		// Set ALS integration time to 100ms
		{0x00, 0x2e, 0x01},		// Performs a single temp calibration of the ranging sensor
		{0x00, 0x1b, 0x00},		// Minimizes time between ranging measurements in continuous mode
		{0x00, 0x3e, 0x31},		// Sets the time between ALS measurements to .5 seconds
		{0x00, 0x14, 0x04},		// Configures interrupt on new sample ready threshold event for only ranging sensor
		{0x00, 0x2d, 0x00},
		{0x00, 0x16, 0x00},
		{0x00, 0x18, 0x03}		// Enables and initiates continuous operation
		
		
	};
	
	// For each of the public regs
	for(int i = 0; i < 12; i++)
	{
		// Write the correct values
		while(!I2CBufferAddInstruction(buf, 0x29, 0, &(VL6180XCustomInitData[i][0]), 3));
	}
}


int main(void)
{
	MCUCR |= (1<<JTD);
	CLKPR = (1 << CLKPCE);
	CLKPR = 0;
	
	I2CInit();
    usartInit();
	
	DDRC |= (1 << DDC6);
	
	
	I2CBuffer_pT ibt = I2CBufferNew();
	if (!ibt)
	{
		// Freak out ahahahahahah
	}
	
	VL6180XInit(ibt);
	//VL6180XInit(ibt);
	
	uint8_t VL6180XRangeDataLocation[2] = {0x00, 0x4f};
	uint8_t VL6180XRangeResultLocation[2] = {0x00, 0x62};
	uint8_t VL6180XIntClear[3] = {0x00, 0x15, 0x07};
	uint8_t VL6180XRESET[3] = {0x00, 0x18, 0x03};
		
	
	volatile uint8_t * shouldRead = calloc(1, sizeof(uint8_t));
	volatile uint8_t * dat = calloc(1, sizeof(uint8_t));
	*shouldRead = 10;
	int32_t loop = 0;
	int count = 0;
	
	I2CSetCurBuf(ibt);
	
    while (1) 
    {
		I2CTask();
		usartTask();
		
		if (!I2CBufferGetCurrentSize(ibt))
		{
			I2CBufferAddInstruction(ibt, 0x29, 0, &(VL6180XRangeDataLocation[0]), 2);		// Tell the sensor we want to read an ID reg
			I2CBufferAddInstruction(ibt, 0x29, 1, shouldRead, 1);	// Read the ID reg
			I2CBufferAddInstruction(ibt, 0x29, 0, &VL6180XIntClear[0], 3);
			count++;
		}
		loop++;
		
		if (*shouldRead)
		{
			I2CBufferAddInstruction(ibt, 0x29, 0, &(VL6180XRangeResultLocation[0]), 2);
			I2CBufferAddInstruction(ibt, 0x29, 1, dat, 1);
			I2CBufferAddInstruction(ibt, 0x29, 0, &VL6180XIntClear[0], 3);
			*shouldRead = 0;
		}	
			
		if (loop > 20000)
		{
			if (*dat == 0)
			{
				I2CBufferAddInstruction(ibt, 0x29, 0, &VL6180XRESET[0], 3);
			}
			char out[50];
			sprintf(out, "%d %d %d %d\n", *shouldRead, *dat, I2CBufferGetCurrentSize(ibt), *(I2CInstructionGetData(I2CBufferGetCurrentInstruction(ibt)) + I2CInstructionGetLength(I2CBufferGetCurrentInstruction(ibt)) - 1));
			addStringToUsartWriteBuffer(out);
			loop = 0;
			*shouldRead = 0;
			*dat = 0;
		}
		
    }
}

