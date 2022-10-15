This repository contains code for using the TWI peripheral for I2C on an AVR microcontroller. 

I believe the registers used are consistant for all AVR chips, but just in case, be warned that this was developed and tested on an ATMega32u4.

I built this because I was frustrated with having to write an I2C driver each time I wanted to use I2C. In addition, I found most existing I2C libraries to be slow because the I2C write and read functions would block until they finished a transaction. This library does not block program execution, or in other words, in between handling the many parts of an I2C transaction, other instructions can be executed.

All requisite I2C functionality is built into the I2CInstruction struct (abstracted from the user) which contains the I2C address to be written to, the data to be written (or a memory location to read the data to), whether the transaction is a read or a write, the number of bytes to be transmitted or received, and an ID so that pointers aren't flying around and there is no ambiguity regarding memory safety.

Users interact with the I2CInstructions via an I2CBuffer struct, which has a user accessible API. Users can add instructions to the buffer, remove instructions (via their id), check if an instruction is in a buffer (based on it's id), check the size of the buffer, etc. Buffers are implemented as linked lists, and are dynamically allocated with a maximum size defined in the header files.

See documentation.txt for explanations/usecases
