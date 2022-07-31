/*
 * I2C.h
 *
 * Created: 5/26/2022 4:28:23 PM
 *  Author: Jack2bs
 */ 


#ifndef I2C_DRIVER_H_
#define I2C_DRIVER_H_

#include <avr/io.h>

#include "I2CInstruction.h"

// TWCR Macros

#define TWI_INT_FLAG	TWINT				// I2C interrupt flag
#define TWI_ACK_EN		TWEA				// I2C ACK Enable
#define TWI_START		TWSTA				// I2C Send start bit
#define TWI_STOP		TWSTO				// I2C Send stop bit
#define TWI_WRI_COL		TWWC				// I2C Write collision bit
#define TWI_ENABLE		TWEN				// I2C Enable
#define TWI_INT_EN		TWIE				// I2C interrupt enable

// I2C States

#define START_TRA					0x08	// Start transmitted
#define REP_START_TRA				0x10	// Repeated start transmitted
#define SLA_W_TRA_ACK_REC			0x18	// Slave address + write transmitted and an ACK received
#define SLA_W_TRA_NACK_REC			0x20	// Slave address + write transmitted and a NACK received
#define DATA_TRA_ACK_REC			0x28	// A data byte has been transmitted and an ACK received
#define DATA_TRA_NACK_REC			0x30	// A data byte has been transmitted and a NACK received
#define ARB_LOST					0x38	// Arbitration has been lost
#define SLA_R_TRA_ACK_REC			0x40	// Slave address + read transmitted and an ACK received
#define SLA_R_TRA_NACK_REC			0x48	// Slave address + read transmitted and a NACK received
#define DATA_REC_ACK_TRA			0x50	// Data received and ACK transmitted
#define DATA_REC_NACK_TRA			0x58	// Data received and NACK transmitted
#define SLA_W_REC_ACK_TRA			0x60	// Our Slave_add + w received and ACK transmitted
#define ARB_LOST_SLA_W_REC_ACK_TRA	0x68	// Our Slave_add + w received and NACK transmitted
#define GEN_CALL_ACK_TRA			0x70	// Gen call received and ACK transmitted
#define ARB_LOST_GEN_CALL_ACK_TRA	0x78	// Arbitration lost, gen call received, ACK transmitted
#define SLA_W_DATA_REC_ACK_TRA		0x80	// As slave, data received, ACK transmitted
#define SLA_W_DATA_REC_NACK_TRA		0x88	// As slave, data received, NACK transmitted
#define GEN_CALL_DATA_REC_ACK_TRA	0x90	// As gen call recipient, data received, ACK transmitted
#define GEN_CALL_DATA_REC_NACK_TRA	0x98	// As gen call recipient, data received, NACK transmitted
#define STOP_OR_REP_START_REC		0xA0	// As slave, stop or rep start received
#define SLA_R_REC_ACK_TRA			0xA8	// Slave_add + r received and ACK transmitted
#define ARB_LOST_SLA_R_REC_ACK_TRA	0xB0	// Arbitration lost, Sla_add + r received, ACK transmitted
#define SLA_DATA_TRA_ACK_REC		0xB8	// As slave, data transmitted, ACK received
#define SLA_DATA_TRA_NACK_REC		0xC0	// As slave, data transmitted, NACK received
#define LAST_DATA_TRA_ACK_REC		0xC8	// As slave, last data transmitted, ACK received

/*End API*/

void I2CTask();		// Called every loop to determine when to start I2C transaction		
void I2CHandle();	// This handles I2C using info from the I2C-Instructions
void I2CInit();		// Initialize the I2C 
void I2CSetCurBuf(I2CBuffer_pT buf);

#endif /* I2C_DRIVER_H_ */