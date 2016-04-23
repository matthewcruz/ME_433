// ***** file: spicomm.h*****//
#ifndef SPICOMM_H //include guard
#define SPICOMM_H

//****constants****
extern volatile int pwm_val;

//****FUNCTION prototypes*****
void initSPI1();
void initI2C2();
char SPI1_IO(char write);
void setVoltage(char channel, char voltage);
void initExpander();
void setExpander(char pin, char level);
char getExpander();

#endif
