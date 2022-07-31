/*
 * I2CInstruction.h
 *
 * Created: 7/19/2022 1:51:07 AM
 *  Author: jack2
 */ 


#ifndef I2CINSTRUCTION_H_
#define I2CINSTRUCTION_H_

#include <string.h>

#define I2C_WRITE	0
#define I2C_READ	1

// Custom I2C instruction Struct
typedef struct I2CInstruction * I2CInstruction_pT;

I2CInstruction_pT I2CInstructionNew(int d_add, int rw, uint8_t* dat, int leng);
void I2CInstructionFree(I2CInstruction_pT ipt);
int I2CInstructionGetAddress(I2CInstruction_pT ipt);
int I2CInstructionGetLength(I2CInstruction_pT ipt);
uint8_t * I2CInstructionGetData(I2CInstruction_pT ipt);
int I2CInstructionGetReadWrite(I2CInstruction_pT ipt);
I2CInstruction_pT I2CInstructionGetNextInstr(I2CInstruction_pT ipt);


typedef struct I2CBuffer * I2CBuffer_pT;

I2CBuffer_pT I2CBufferNew();
void I2CBufferFree(I2CBuffer_pT buf);
I2CInstruction_pT I2CBufferMoveToNextInstruction(I2CBuffer_pT buf);
I2CInstruction_pT I2CBufferGetCurrentInstruction(I2CBuffer_pT buf);
I2CInstruction_pT I2CBufferPushInstruction(I2CBuffer_pT buf, I2CInstruction_pT newInstr);
I2CInstruction_pT I2CBufferAddInstruction(I2CBuffer_pT buf, int d_add, int rw, uint8_t* dat, int leng);
size_t I2CBufferGetCurrentSize(I2CBuffer_pT buf);
int I2CBufferContains(I2CBuffer_pT buf, I2CInstruction_pT instr);
int I2CBufferRemove(I2CBuffer_pT buf, I2CInstruction_pT instr);
void I2CBufferSendToBack(I2CBuffer_pT buf);


#endif /* I2CINSTRUCTION_H_ */