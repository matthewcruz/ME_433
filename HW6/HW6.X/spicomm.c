#include<xc.h>           // processor SFR definitions
#include<sys/attribs.h>  // __ISR macro
#include "spicomm.h"
#define CS LATBbits.LATB7 //chip select pin

//***initialization functions
void initSPI1(){

    //***output pin config
    RPB8Rbits.RPB8R = 0b0011; // set SDO1 to RPB8 (PIC pin # 17)
    //RPB7Rbits.RPB7R = 0b0011; // set ss1 to RPB7 (PIC pin #16)
    //***input pin config Example INT4Rbits.INT4R = 0b0011; assigns INT4 to pin B15.
    SDI1Rbits.SDI1R = 0b0000;   //assign SDI1 to pin RPA1
    
    CS =1;
    SPI1CON = 0;                //turn off the spi module and reset it
    SPI1BUF;                    //clear the RX buffer by reading from it
    SPI1STATbits.SPIROV = 0;
    SPI1CONbits.CKE = 1;        //data changes when clock goes from hi to lo
    SPI1CONbits.MSTEN = 1;    //master operation
    SPI1BRG= 1;               //this is 24KHz
    SPI1CONbits.ON = 1;         //turn on the SPII1 line

}

char SPI1_IO(unsigned char write){
    SPI1BUF = write;
    while(!SPI1STATbits.SPIRBF) {;} // wait to receive the byte
    return SPI1BUF;
}

void setVoltage(char channel, unsigned char voltage) {
    unsigned short buff = 0x00;
    buff = buff | channel << 15;
    buff = buff | 0x7 << 12;
    buff = buff | voltage <<4;
   
   
    CS = 0; //need pullup resistor on chip select?
    SPI1_IO((buff & 0xFF00)>>8);
    SPI1_IO(buff & 0x00FF);
    CS = 1;
      
}




