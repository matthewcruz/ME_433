#include<xc.h>           // processor SFR definitions
#include<sys/attribs.h>  // __ISR macro
#include<i2c_master_noint.h> //neeed to create th .h file for the .c file i2c_master_noint.c
#include<spicomm.h>
#define CS LATBbits.LATB8 //chip select pin

//***initialization functions
void initSPI1(){

    //***output pin config
    RPB8Rbits.RPB8R = 0b0011; // set SDO1 to RPB8 (PIC pin # 17)
    RPB7Rbits.RPB7R = 0b0011; // set ss1 to RPB7 (PIC pin #16)
    
    //***input pin config Example INT4Rbits.INT4R = 0b0011; assigns INT4 to pin B15.
    SDI1Rbits.SDI1R = 0b0000; //assign SDI1 to pin RPA1
    
    //CS = 1;                    //when CS goes hi chip select is off
    SPI1CON = 0;               //turn off the spi module and reset it
    SPI1BUF;                   //clear the RX buffer by reading from it
    SPI1CONbits.MODE32 = 0b00; //set the mode of communication to 8 bit data
    SPI1CONbits.SPIROV = 0;
    SPI1CONbits.CKE = 1;       //data changes when clock goes from hi to lo
    //SPI1CONbits.MSTEN = 0;     //master operation
    SPI1BRG= 0x1;              //this is 12 MHz
    SPI1CONbits.ON = 1;        //turn on the SPII1 line
    //when to use SPI1CONbits.SSEN??
    
}

void initI2C2(){
    ANSELBbits.ANSB2 = 0; // resets the I2C pins so that they are not analog
    //**** Need to fill out value in i2c_master_noit.c to set I2C1BRG = ??? for speed
    i2c_master_setup(); // need to include i2c_master_noint.c this activates I2C1 as sets SDA and SCL pins 
}

char SPI1_IO(char write){
    //SPI1CONbits.SSEN = 1; //otherwise we can control with using port??
    SPI1BUF = write;
    while(!SPI1STATbits.SPIRBF) {;} // wait to receive the byte
    return SPI4BUF;

}

void setVoltage(char channel, char voltage) {
    //use example from code provided by nick here
    //will likely set the values to some buffers to be written over I2C or SPI
    channel = ???// two channels
    voltage = ??? // some function with relation to time    
}

void initExpander(){
}


void setExpander(char pin, char level){
}


char getExpander(){
}






