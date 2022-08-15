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

/* I2CInstruction_pT is a pointer to an I2CInstruction structure */
typedef struct I2CInstruction * I2CInstruction_pT;

/* Returns ipt.dev_addr */
int I2CInstructionGetAddress(I2CInstruction_pT ipt);

/* Returns ipt.length */
int I2CInstructionGetLength(I2CInstruction_pT ipt);

/* Returns ipt.data */
uint8_t * I2CInstructionGetData(I2CInstruction_pT ipt);

/* Returns ipt.readWrite */
int I2CInstructionGetReadWrite(I2CInstruction_pT ipt);

/* Returns ipt.nextInstr */
I2CInstruction_pT I2CInstructionGetNextInstr(I2CInstruction_pT ipt);

/* I2CBuffer_pT is a pointer to an I2CBuffer structure */
typedef struct I2CBuffer * I2CBuffer_pT;

/* I2CBuffer constructor. Returns a pointer to a new I2CBuffer or NULL is the operation failed */
I2CBuffer_pT I2CBufferNew();

/* I2CBuffer destructor. Frees all memory associated with an I2CBuffer. In all likelihood, never necessary as Buffers should last until program completion */
void I2CBufferFree(I2CBuffer_pT buf);

/* Frees the current instruction, Moves buf.currPt to the next instruction, returns the NEW buf.currPt (NULL if the operation failed) */
I2CInstruction_pT I2CBufferMoveToNextInstruction(I2CBuffer_pT buf);

/* Returns buf.currPt (See I2CBuffer struct) */
I2CInstruction_pT I2CBufferGetCurrentInstruction(I2CBuffer_pT buf);

/* Adds a new instruction to the end of buf, where the new instruction has the following data
 * dev_addr = d_add
 * readWrite = rw
 * data = dat
 * length = leng
 * nextInstr = NULL */
I2CInstruction_pT I2CBufferAddInstruction(I2CBuffer_pT buf, int d_add, int rw, uint8_t* dat, int leng);

/* Returns buf.currentSize (See I2CBuffer struct) */
size_t I2CBufferGetCurrentSize(I2CBuffer_pT buf);

/* Returns 1 (true) if buf contains instr, or 0 (false) if buf does not contain instr */
int I2CBufferContains(I2CBuffer_pT buf, I2CInstruction_pT instr);

/* Removes instr from buf if buf contains instr. Returns 1 if buf contained instr, 0 otherwise */
int I2CBufferRemove(I2CBuffer_pT buf, I2CInstruction_pT instr);

/* Moves the current value of buf.currPt to buf.endPt and sets buf.currPt to the next instruction */
void I2CBufferSendToBack(I2CBuffer_pT buf);


#endif /* I2CINSTRUCTION_H_ */