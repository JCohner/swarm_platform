#ifndef I2C_MASTER_INT__H__
#define I2C_MASTER_INT__H__

// buffer pointer type.  The buffer is shared by an ISR and mainline code.
// the pointer to the buffer is also shared by an ISR and mainline code.
// Hence the double volatile qualification
typedef volatile unsigned char * volatile buffer_t;

void i2c_master_setup(); //sets up I2C1 as a master using an interrupt

// Initiate an I2C write read operation at the given address. 
// You can optionally only read or only write by passing 0 length for reading or writing.
// This will not return until the transaction is complete.  Returns false on error.
int i2c_write_read(unsigned int addr, const buffer_t write, unsigned int wlen, 
    const buffer_t read, unsigned int rlen );

// write a single byte to the slave
int i2c_write_byte(unsigned int addr, unsigned char byte);

#endif
