// I2C Master utilities, 100 kHz, using polling rather than interrupts
// I2C pins need pull-up resistors, 2k-10k
#include <xc.h>
#include "i2c_master_noint.h"


char add= 0x20;
char reg = 0x00;
unsigned char outval = 0xF0;
unsigned char data[30];
char len;

//can include ISR here or in main

void initI2C2(){
    //B2 = PIN 6 SDA2, B3 = PIN 7, SCL2, need to reassign and turn off analog
    ANSELBbits.ANSB2 = 0; // resets the I2C pins so that they are not analog
    ANSELBbits.ANSB3 = 0;
    i2c_master_setup(); 
}

void initOC(){
    //***output pin config for OC1 and OC2 for PWM function
    RPA0Rbits.RPA0R = 0b0101; // set OC1 to RPA0 (PIC pin # 2)
    RPA1Rbits.RPA1R = 0b0101; // set OC2 to RPA1 (PIC pin # 3)
    int sval = 1500;
    // start T2 used for OC1 and OC2
    T2CONbits.TCKPS = 4;     // Timer2 PReSc N=16 0b100
	PR2 = 2999;              // period = (PR2+1) * N * 20.833 ns = 1000 us, 1 kHz
	TMR2 = 0;                // initialize at 0
	T2CONbits.ON = 1;        // turn on Timer2
	//Output compare at OC1
	OC1CONbits.OCM = 0b110;  	//PWM mode without fault pin; other OC1CON bits are defaults
	OC1CONbits.OC32 = 0;		//use the 16 bit timer
	OC1RS = sval;             	// duty cycle = OC1RS/(PR3+1) = 25%
	OC1R = sval;              	// initialize before turning OC1 on; afterward it is read-only
	OC1CONbits.OCTSEL = 0;  	//Use timer 2
	OC1CONbits.ON = 1;       	// turn on OC1
    //Output compare at OC2
	OC2CONbits.OCM = 0b110;  	//PWM mode without fault pin; other OC1CON bits are defaults
	OC2CONbits.OC32 = 0;		//use the 16 bit timer
	OC2RS = sval;             	// duty cycle = OC1RS/(PR3+1) = 25%
	OC2R = sval;              	// initialize before turning OC1 on; afterward it is read-only
	OC2CONbits.OCTSEL = 0;  	//Use timer 2
	OC2CONbits.ON = 1;       	// turn on OC2
    //SET Interrupt for TIMER2
  	IPC2bits.T2IP = 5;            // interrupt priority default sub
	IFS0bits.T2IF = 0;            // clear the int flag
	IEC0bits.T2IE = 1;            // enable T2 as interrupt by setting IEC0
}

void initGyro(){
    //write to CTRL1_XL to initialize ACCELEROMETER
    add = 0x6B;// 1101011b for gyro
    reg = 0x10; //CTRL1_XL set all values to initialize
    outval = 0x80;  // 0b10000000  Out Data Rate is 1.66Khz or 0b1000 Most Sb FS_XL1 = 0b00 (+- 2g) BW_XL anti aliasing filter bandwidth 0b00
    i2c_master_start(); // make the start bit
    i2c_master_send(add<<1|0); // write the address, shifted left by 1, or'ed with a 0 to indicate writing
    i2c_master_send(reg); // the register to write to
    i2c_master_send(outval); // the value to put in the register
    i2c_master_stop(); // make the stop bit
    //write to CTRL2_G TO INITIALIZE GYRO
    reg = 0x11; 
    outval = 0x84; // 0b10000100gyro FS_G = 0b01 for 500 dp gyro ODR_G 1.66 KHz = 0b1000
    i2c_master_start(); // make the start bit
    i2c_master_send(add<<1|0); // write the address, shifted left by 1, or'ed with a 0 to indicate writing
    i2c_master_send(reg); // the register to write to
    i2c_master_send(outval); // the value to put in the register
    i2c_master_stop(); // make the stop bit
}

void I2C_multiread(char add, char reg, unsigned char * data, char len){
    i2c_master_start(); // make the start bit
    i2c_master_send(add<<1|0); // write the address, shifted left by 1, or'ed with a 0 to indicate writing
    i2c_master_send(reg); // the register to write to
    i2c_master_restart();
    i2c_master_send(add<<1 |1); // the value to put in the register
    int i;
    for(i=0;i<len-1;i++){
        data[i]= i2c_master_recv();
        i2c_master_ack(0);
    }
    data[len-1]=i2c_master_recv();
    i2c_master_ack(1); // make 
    i2c_master_stop();
}

void initExpander(){
    //set the expander pins using IODIR registrt at address 0x00  bit=1 is input
    //address is set with 0100A2A1A0, where each A is set externally, set all high
    //add then becomes 0100111
    add = 0x20;
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
    add = 0x20;
    reg = 0x09; //GPIO
    outval = level << pin;
    i2c_master_start(); // make the start bit
    i2c_master_send(add<<1|0); // write the address, shifted left by 1, or'ed with a 0 to indicate writing
    i2c_master_send(reg); // the register to write to
    i2c_master_send(outval); // the value to put in the register
    i2c_master_stop(); // make the stop bit
}


unsigned char getWho(){
    //char add = 0x20;//0b0100000;
    unsigned char r = 0x00;
    i2c_master_start(); // make the start bit
    i2c_master_send(0x6B<<1|0); // write the address, shifted left by 1, or'ed with a 0 to indicate writing
    i2c_master_send(0x0f); // the register to read from
    i2c_master_restart(); // make the restart bit
    i2c_master_send(0x6b<<1|1); // write the address, shifted left by 1, or'ed with a 1 to indicate reading
    r = i2c_master_recv();
    i2c_master_ack(1); // make the ack so the slave knows we got it
    i2c_master_stop(); // make the stop bit
    return r;

}

char getExpander(){
    //char add = 0x20;//0b0100000;
    add = 0x20;
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
                                    //53 gives 400 KHz i think 233 gives 100 KHz
  I2C2CONbits.ON = 1;                // turn on the I2C2 module
  
}

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
