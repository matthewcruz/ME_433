//This code takes imu data and feeds it to the mouse


#include <xc.h>           // processor SFR definitions
#include <sys/attribs.h>  // __ISR macro
#include "math.h"
#include "i2c_master_noint.h"

//Global Variables
signed short temp = 0,accel_x= 0, accel_y=0, accel_z=0, gyro_x=0, gyro_y=0, gyro_z=0;



// OC1RS = 3000*(float)(accel_x + (0xFFFF/2) + 1)/0XFFFF; // OC1RS = 1500;
// OC2RS = 3000*(float)(accel_y + (0xFFFF/2) + 1)/0xFFFF; // OC2RS = 1500;

 __builtin_disable_interrupts();
initI2C2();
initGyro();
__builtin_enable_interrupts();

_CP0_SET_COUNT(0);

//Declare variables
char voltage1=0, chann=0;
float spi_scale = 122.0;
unsigned char wave[1000], triangle[1000];
int level = 0, i = 0;
unsigned char val = 0x00;

while(1){
        
        
        if (_CP0_GET_COUNT()>240000){
        //LATAbits.LATA4 = 1;
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
        

        }       

}
 
}
