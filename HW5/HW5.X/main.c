#include <xc.h>           // processor SFR definitions
#include <sys/attribs.h>  // __ISR macro
#include "math.h"
//#include "spicomm.h"
#include "i2c_master_noint.h"
#include "ILI9163C.h"
#include <stdlib.h>
#include <stdio.h>

// DEVCFG0
#pragma config DEBUG = OFF // no debugging
#pragma config JTAGEN = OFF // no jtag
#pragma config ICESEL = ICS_PGx1 // use PGED1 and PGEC1
#pragma config PWP = OFF// no write protect
#pragma config BWP = OFF // no boot write protect
#pragma config CP = OFF // no code protect

// DEVCFG1
#pragma config FNOSC = PRIPLL // use primary oscillator with pll
#pragma config FSOSCEN = OFF // turn off secondary oscillator
#pragma config IESO = OFF // no switching clocks
#pragma config POSCMOD = HS // high speed crystal mode
#pragma config OSCIOFNC = OFF// free up secondary osc pins
#pragma config FPBDIV = DIV_1 // divide CPU freq by 1 for peripheral bus clock
#pragma config FCKSM = CSDCMD // do not enable clock switch
#pragma config WDTPS = PS1048576 // slowest wdt
#pragma config WINDIS = OFF // no wdt window
#pragma config FWDTEN = OFF // wdt off by default
#pragma config FWDTWINSZ = WINSZ_25 // wdt window at 25%

// DEVCFG2 - get the CPU clock to 48MHz
#pragma config FPLLIDIV = DIV_2 // divide input clock to be in range 4-5MHz
#pragma config FPLLMUL = MUL_24 // multiply clock after FPLLIDIV
#pragma config FPLLODIV = DIV_2 // divide clock after FPLLMUL to get 48MHz
#pragma config UPLLIDIV = DIV_2 // divider for the 8MHz input clock, then multiply by 12 to get 48MHz for USB
#pragma config UPLLEN = ON // USB clock on

// DEVCFG3
#pragma config USERID = 33 // some 16bit userid, doesn't matter what
#pragma config PMDL1WAY = OFF // allow multiple reconfigurations
#pragma config IOL1WAY = OFF // allow multiple reconfigurations
#pragma config FUSBIDIO = ON // USB pins controlled by USB module
#pragma config FVBUSONIO = ON // USB BUSON controlled by USB module

//Global Variables
signed short temp = 0,accel_x= 0, accel_y=0, accel_z=0, gyro_x=0, gyro_y=0, gyro_z=0;
unsigned short color;

//void __ISR(_TIMER_2_VECTOR, IPL5SOFT) PWM(void){
//    OC1RS = 3000*(float)(accel_x + (0xFFFF/2) + 1)/0XFFFF; // OC1RS = 1500;
//    OC2RS = 3000*(float)(accel_y + (0xFFFF/2) + 1)/0xFFFF; // OC2RS = 1500;
//    IFS0bits.T2IF = 0;
//}

void disp_char(unsigned short xo, unsigned short yo, char input){
    color = RED;
    int j, k;
    char col; 
    
    for (j=0; j<5;j++){ // each column
        col = ASCII[input-32][j];
            for (k=0; k<8; k++){ //kth bit in column
                if ((col >> k) & 0x01 == 1) {
                    LCD_drawPixel(xo + j, yo + k, color);
                }
                else{
                       LCD_drawPixel(xo + j, yo + k, WHITE);
                    }
                
            }
    }
  
}
int main() {

    __builtin_disable_interrupts();

    // set the CP0 CONFIG register to indicate that kseg0 is cacheable (0x3)
    __builtin_mtc0(_CP0_CONFIG, _CP0_CONFIG_SELECT, 0xa4210583);

    // 0 data RAM access wait states
    BMXCONbits.BMXWSDRM = 0x0;

    // enable multi vector interrupts
    INTCONbits.MVEC = 0x1;

    // disable JTAG to get pins back
    DDPCONbits.JTAGEN = 0;
    
    // do your TRIS and LAT commands here
    //set port4A to digital out
    TRISAbits.TRISA4 = 0;
    LATAbits.LATA4 = 0;
    TRISBbits.TRISB7 = 0;
     
    //initialize SPI and I2C and Output compare
    //initOC(); //remap pins 2,3 (A0 and A1) to OC1 and OC2
    SPI1_init();
    initI2C2();
    initGyro();
    LCD_init();
    __builtin_enable_interrupts();
    
    _CP0_SET_COUNT(0);
    
    //Declare variables
    char voltage1=0, chann=0;
    float spi_scale = 122.0;
    unsigned char wave[1000], triangle[1000], val = 0x00;
    int level = 0, i = 0;
    char message[30];
    //sprintf(message, "Hello world 1337!");
    
    //finish variable declaration
    
    
    //make a triangle and sin wave
    for (i=0; i<1000; i++){
        wave[i] = (unsigned char)(123 + spi_scale*sin(6.28*i/(1000.)));
        triangle[i] = (unsigned char)255*i/1000.;
    }
    LCD_clearScreen(WHITE);
    //do this once for the lcd 
    i = 0;
    sprintf(message, "X is: ");
    while(message[i]){
        disp_char(1 + i*6, 3, message[i]);
        // move to next location on screen
        i++;
    }
    sprintf(message, "Y is: ");
    while(message[i]){
        disp_char(1 + i*6, 3+9, message[i]);
        // move to next location on screen
        i++;
    }
    
    while (_CP0_GET_COUNT()<2400000){
        ;
    }
    _CP0_SET_COUNT(0);
    
    while(1){
        
            if (_CP0_GET_COUNT()>240000){
            val = getWho();
                if (val==0x69 ){
                     LATAbits.LATA4 = !LATAbits.LATA4;
                }
            _CP0_SET_COUNT(0);
            
            I2C_multiread(0x6B, 0x20, data, 14); //may not even need to pass data here and can just save in the externed variable
            temp =(data[1]<< 8) | data[0];
            gyro_x =(data[3]<< 8) | data[2];
            gyro_y =(data[5]<< 8) | data[4];
            gyro_z =(data[7]<< 8) | data[6];
            accel_x =(data[9]<< 8) | data[8];
            accel_y =(data[11]<< 8) | data[10];
            accel_z =(data[13]<< 8) | data[12];
            
            i = 0;
            sprintf(message, "%1.2f", -2*9.81 + 9.81*4*(float)(accel_x + (0xFFFF/2) + 1)/0XFFFF);
            while(message[i]){
                disp_char(7*6 +i*6, 3, message[i]);
                // move to next location on screen
                i++;
            }
            sprintf(message, "%1.2f", -2*9.81 + 9.81*4*(float)(accel_y + (0xFFFF/2) + 1)/0XFFFF);
            i=0;
            while(message[i]){
                disp_char(7*6 +i*6, 3+9, message[i]);
                // move to next location on screen
                i++;
            }
            

            }       

    }
     
}
