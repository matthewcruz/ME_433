// ***** file: spicomm.h*****//
#ifndef SPICOMM_H //include guard
#define SPICOMM_H

//****constants****


//****FUNCTION prototypes*****
void initSPI1();
char SPI1_IO(unsigned char write);
void setVoltage(char channel, unsigned char voltage);

#endif
