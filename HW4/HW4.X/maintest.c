#include <xc.h>           // processor SFR definitions
#include <sys/attribs.h>  // __ISR macro
#include "math.h"
#include "spicomm.h"
#include "i2c_master_noint.h"

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
    LATAbits.LATA4 = 1;
    TRISBbits.TRISB7 = 0;
    _CP0_SET_COUNT(0);
    
    __builtin_enable_interrupts();
    
    //initialize SPI and I2C
    initSPI1();
    initI2C2();
    initExpander();
    
    //Declare variables
    char voltage1=0, chann=0;
    float spi_scale = 122.0;
    unsigned char wave[1000];
    unsigned char triangle[1000];
    int level = 0;
    //end SPI initialization
    int i = 0;
    for (i=0; i<1000; i++){
        wave[i] = (unsigned char)(123 + spi_scale*sin(6.28*i/(1000.)));
        triangle[i] = (unsigned char)255*i/1000.;
    }
    i=0;
    
    while(1){

//        if (_CP0_GET_COUNT()>2400){
////            if (level == 0){
////                level =1;
////            }
//            level = (level^1) & 0x1;
////            else{ level = 0;}
//            setExpander(0, level);
//            setVoltage(0, wave[i]);   
//            setVoltage(1,triangle[i]);
//            LATAbits.LATA4 = !LATAbits.LATA4; //change LED state
//
//            _CP0_SET_COUNT(0);
//            i++;
//        }
        if (i>999){
            i=0;
        }
        
        if ((getExpander()>>7)&0x01 == 1){
            setExpander(0,1);
        }
        else {setExpander(0,0);}
        
                
    }
    
    
}
