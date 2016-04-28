// I2C Master utilities, 100 kHz, using polling rather than interrupts
// The functions must be callled in the correct order as per the I2C protocol
// **DONEChange I2C1 to the I2C channel you are using 
// I2C pins need pull-up resistors, 2k-10k
#include <xc.h>
#include "i2c_master_noint.h"

#define add 0x20
char reg = 0x00;
char outval = 0xF0;

void initI2C2(){
    //notes - I2C2 
    //B2 = PIN 6 SDA2, B3 = PIN 7, SCL2, need to reassign and turn off analog
    ANSELBbits.ANSB2 = 0; // resets the I2C pins so that they are not analog
    ANSELBbits.ANSB3 = 0;
    //**** Need to fill out value in i2c_master_noint.c to set I2C1BRG = ??? for speed
    i2c_master_setup(); // need to include i2c_master_noint.c this activates I2C1 as sets SDA and SCL pins 
}

void initExpander(){
    //set the expander pins using IODIR registrt at address 0x00  bit=1 is input
    //address is set with 0100A2A1A0, where each A is set externally, set all high
    //add then becomes 0100111
    reg = 0x00;
    outval = 0xF0; //same as 0b11110000
    //char add = 0x20;//0b0100000;
    i2c_master_start(); // make the start bit
    i2c_master_send(add<<1|0); // write the address, shifted left by 1, or'ed with a 0 to indicate writing
    i2c_master_send(reg); // the register to write to
    i2c_master_send(outval); // the value to put in the register
    i2c_master_stop(); // make the stop bit
    
    i2c_master_start(); // make the start bit
    i2c_master_send(add<<1|0); // write the address, shifted left by 1, or'ed with a 0 to indicate writing
    i2c_master_send(0x06); // the register to write to
    i2c_master_send(0xF0); // the value to put in the register
    i2c_master_stop(); // make the stop bit
    
    
    
}

void setExpander(char pin, char level){
    //char add = 0x20;//0b0100000;
    reg = 0x09; //GPIO
    outval = level << pin;
    i2c_master_start(); // make the start bit
    i2c_master_send(add<<1|0); // write the address, shifted left by 1, or'ed with a 0 to indicate writing
    i2c_master_send(reg); // the register to write to
    i2c_master_send(outval); // the value to put in the register
    i2c_master_stop(); // make the stop bit
}


char getExpander(){
    //char add = 0x20;//0b0100000;
    i2c_master_start(); // make the start bit
    i2c_master_send(add<<1|0); // write the address, shifted left by 1, or'ed with a 0 to indicate writing
    i2c_master_send(reg); // the register to read from
    i2c_master_restart(); // make the restart bit
    i2c_master_send(add<<1|1); // write the address, shifted left by 1, or'ed with a 1 to indicate reading
    char r = i2c_master_recv(); // save the value returned
    i2c_master_ack(1); // make the ack so the slave knows we got it
    i2c_master_stop(); // make the stop bit
    return r;
}



void i2c_master_setup(void) {
  I2C2BRG = 53;//233;//???some number for 100kHz;  using Fsck as what i want and Pblck as resonator clock
                                    // I2CBRG = [1/(2*Fsck) - PGD]*Pblck - 2 
                                    // look up PGD for your PIC32 PGD is between 50 and 300 ns
  I2C2CONbits.ON = 1;                // turn on the I2C2 module
}

// Start a transmission on the I2C bus
void i2c_master_start(void) {
    I2C2CONbits.SEN = 1;            // send the start bit
    while(I2C2CONbits.SEN) { ; }    // wait for the start bit to be sent
}

void i2c_master_restart(void) {     
    I2C2CONbits.RSEN = 1;           // send a restart 
    while(I2C2CONbits.RSEN) { ; }   // wait for the restart to clear
}

void i2c_master_send(unsigned char byte) { // send a byte to slave
  I2C2TRN = byte;                   // if an address, bit 0 = 0 for write, 1 for read
  while(I2C2STATbits.TRSTAT) { ; }  // wait for the transmission to finish
  if(I2C2STATbits.ACKSTAT) {        // if this is high, slave has not acknowledged
    // ("I2C2 Master: failed to receive ACK\r\n");
  }
}

unsigned char i2c_master_recv(void) { // receive a byte from the slave
    I2C2CONbits.RCEN = 1;             // start receiving data
    while(!I2C2STATbits.RBF) { ; }    // wait to receive the data
    return I2C2RCV;                   // read and return the data
}

void i2c_master_ack(int val) {        // sends ACK = 0 (slave should send another byte)
                                      // or NACK = 1 (no more bytes requested from slave)
    I2C2CONbits.ACKDT = val;          // store ACK/NACK in ACKDT
    I2C2CONbits.ACKEN = 1;            // send ACKDT
    while(I2C2CONbits.ACKEN) { ; }    // wait for ACK/NACK to be sent
}

void i2c_master_stop(void) {          // send a STOP:
  I2C2CONbits.PEN = 1;                // comm is complete and master relinquishes bus
  while(I2C2CONbits.PEN) { ; }        // wait for STOP to complete
}
