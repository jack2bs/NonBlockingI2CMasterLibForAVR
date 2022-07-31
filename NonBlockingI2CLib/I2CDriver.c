/*
 * I2C.c
 *
 * Created: 5/26/2022 4:28:11 PM
 *  Author: Jack2bs
 */ 

// Other includes
#include <avr/interrupt.h>
#include <stdint.h>

// Custom includes
#include "I2CDriver.h"
#include "I2CInstruction.h"
#include "Usart.h"

// I2C event interrupt
ISR(TWI_vect)
{
	I2CHandle();
}


// Sends a start condition to the I2C bus
inline void sendStartCond()
{
	TWCR = (1 << TWI_INT_FLAG) | (1 << TWI_ACK_EN) | (1 << TWI_START) | (1 << TWI_ENABLE) | (1 << TWI_INT_EN);
}

// Sends a stop condition to the I2C bus
inline void sendStopCond()
{
	TWCR = (1 << TWI_INT_FLAG) | (1 << TWI_ACK_EN) | (1 << TWI_STOP) | (1 << TWI_ENABLE) | (1 << TWI_INT_EN);
}

// Enables ACK
inline void enableACK()
{
	TWCR = (1 << TWI_INT_FLAG) | (1 << TWI_ACK_EN) | (1 << TWI_ENABLE) | (1 << TWI_INT_EN);
}

// Disables ACK
inline void disableAck()
{
	TWCR = (1 << TWI_INT_FLAG) | (1 << TWI_ENABLE) | (1 << TWI_INT_EN);
}

// Load data into TWDR
inline void loadTWDR(uint8_t data)
{
	TWDR = data;
	TWCR = (1 << TWI_INT_FLAG) | (1 << TWI_ENABLE) | (1 << TWI_INT_EN);
}

// Read is high on SDA, Write is low on SDA
// Loads the slave address + r/w onto the I2C bus
inline void loadAdress(uint8_t address, uint8_t r_w)
{
	loadTWDR((address << 1) | r_w);
}

// Read is high on SDA
// Loads the slave address + r onto the I2C bus
inline void loadAddressRead(uint8_t address)
{
	loadTWDR((address << 1) | 1);
}

// Write is low on SDA
// Loads the slave address + w onto the I2C bus
inline void loadAddressWrite(uint8_t address)
{
	loadTWDR(address << 1);
}

// This is high when the I2C bus is active and low when its not
static uint8_t g_state = 0;

// This has to exist because we need to access the buffer from interrupts
// Global variables it is :(
static I2CBuffer_pT g_curBuf = NULL;
void I2CSetCurBuf(I2CBuffer_pT buf)
{
	g_curBuf = buf;
}

// This handles I2C using info from the I2C-Instructions
void I2CHandle()
{	
	static I2CInstruction_pT curInst = NULL;			// Holds the current instruction	
	static int dataPtr = 0;								// Holds how many bytes have been written/read
	curInst = I2CBufferGetCurrentInstruction(g_curBuf);

	// Switch for the value of the I2C status Reg
	switch(TWSR & 0b11111000)
	{
		// Start or repeated start
		case START_TRA:
		case REP_START_TRA:
			loadAdress(I2CInstructionGetAddress(curInst), I2CInstructionGetReadWrite(curInst));	// Load the device address and r/w
			break;
			
		// Slave address + write has been transmitted and ACK received
		case SLA_W_TRA_ACK_REC:
			loadTWDR(*(I2CInstructionGetData(curInst)));			// Load the first byte to write into TWDR
			dataPtr = 1;											// Update  dataPtr
			break;
			
		// Slave address + write has been transmitted and NACK received
		case SLA_W_TRA_NACK_REC:
			// Could put an error message here
			sendStopCond();						// Send a stop condition
			I2CBufferMoveToNextInstruction(g_curBuf);		// Move to the next instruction (could comment out)
			dataPtr = 0;
			g_state = 0;						// set g_state to 0 (I2C ready/off)
			return;
		
		// A data byte has been transmitted and an ACK received
		case DATA_TRA_ACK_REC:
			// If all of the bytes have been transmitted
			if(dataPtr == I2CInstructionGetLength(curInst))
			{
				sendStopCond();					// Send a stop condition
				I2CBufferMoveToNextInstruction(g_curBuf);		// Move to the next instruction (could comment out)
				g_state = 0;					// set g_state to 0 (I2C ready/off)
				dataPtr = 0;					// Reset the dataPtr var
				return;
			}
			// Otherwise
			else
			{	
				loadTWDR(*(I2CInstructionGetData(curInst) + dataPtr));	// Load the next byte to write into TWDR
				dataPtr++;								// Increment the dataPtr
			}
			break;
			
		// A data byte has been transmitted and a NACK received
		case DATA_TRA_NACK_REC:
			sendStopCond();					// Send a stop condition
			if (dataPtr == I2CInstructionGetLength(curInst))
			{
				I2CBufferMoveToNextInstruction(g_curBuf);		// Move to the next instruction (could comment out)
			}
			else
			{
				I2CBufferMoveToNextInstruction(g_curBuf);
			}
			g_state = 0;					// set g_state to 0 (I2C ready/off)
			dataPtr = 0;
			return;
			
		// Slave address + read transmitted and an ACK received
		case SLA_R_TRA_ACK_REC:
			// If only 1 byte is going to be read
			if(dataPtr == I2CInstructionGetLength(curInst) - 1)
			{
				disableAck();				// Disable the ACK
			}
			// Otherwise
			else
			{
				enableACK();				// Enable the ACk
			}
			break;
		
		// Slave address + read transmitted and a NACK received
		case SLA_R_TRA_NACK_REC:
			sendStopCond();					// Send a stop condition
			I2CBufferMoveToNextInstruction(g_curBuf);	// Push instruction to back and move to the next instruction (could comment out)
			dataPtr = 0;
			g_state = 0;					// set g_state to 0 (I2C ready/off)
			return;
			
		// Data received and ACK transmitted
		case DATA_REC_ACK_TRA:
			*((I2CInstructionGetData(curInst)) + dataPtr) = TWDR;		// Read in the byte
			dataPtr++;							// Increment dataPtr
			// If we've read as much as we want
			if(dataPtr == I2CInstructionGetLength(curInst) - 1)
			{
				disableAck();					// Disable the ACK
			}
			else								// Otherwise
			{
				enableACK();					// Enable the ACK
			}
			break;
		
		// Data received and NACK transmitted
		case DATA_REC_NACK_TRA:
			*(I2CInstructionGetData(curInst) + dataPtr) = TWDR;	// Read in the byte
			dataPtr = 0;					// Reset the dataPtr var
			sendStopCond();					// Send a stop condition
			I2CBufferMoveToNextInstruction(g_curBuf);		// Move to the next instruction (could comment out)
			dataPtr = 0;
			g_state = 0;					// set g_state to 0 (I2C ready/off)
			return;
			
		// If one of the other statuses pops up
		default:
			sendStopCond();							// Send a stop condition
			I2CBufferMoveToNextInstruction(g_curBuf);			// Push instruction to back and move to the next instruction (could comment out)
			dataPtr = 0;
			g_state = 0;							// set g_state to 0 (I2C ready/off)
			return;
	}
	// If we haven't returned, then make sure g_state is 1
	g_state = 1;
}

// Called every loop to determine when to start I2C transaction
void I2CTask()
{
	
	// If g_state is low and there is an instruction available
	if(!g_state)
	{
		
		if (I2CBufferGetCurrentSize(g_curBuf))
		{
			// Send a start condition and update g_state
			sendStartCond();
			g_state = 1;
		}
	}	
}

// Initialize the I2C
void I2CInit()
{
	/*
	
	 (16000000HZ) / (16 + 2x * 4^TWPS) = 400000
	 
	 so
	 
	 (16 + 2x * 4 ^TWPS) = 40
	 
	 so
	 
	 2x * 4^TWPS = 24
	 x = 3
	 TWPS = 1
	 
	 */
	
	// Set TWBR to 3 (calculation shown above)
	TWBR = 50;
	
	// Initial TWCR settings
	TWCR = (1 << TWI_INT_FLAG) | (1 << TWI_ENABLE) | (1 << TWI_INT_EN);
}