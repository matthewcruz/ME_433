/*******************************************************************************
  MPLAB Harmony Project Main Source File

  Company:
    Microchip Technology Inc.
  
  File Name:
    main.c

  Summary:
    This file contains the "main" function for an MPLAB Harmony project.

  Description:
    This file contains the "main" function for an MPLAB Harmony project.  The
    "main" function calls the "SYS_Initialize" function to initialize the state 
    machines of all MPLAB Harmony modules in the system and it calls the 
    "SYS_Tasks" function from within a system-wide "super" loop to maintain 
    their correct operation. These two functions are implemented in 
    configuration-specific files (usually "system_init.c" and "system_tasks.c")
    in a configuration-specific folder under the "src/system_config" folder 
    within this project's top-level folder.  An MPLAB Harmony project may have
    more than one configuration, each contained within it's own folder under
    the "system_config" folder.
 *******************************************************************************/

// DOM-IGNORE-BEGIN
/*******************************************************************************
Copyright (c) 2013-2014 released Microchip Technology Inc.  All rights reserved.

//Microchip licenses to you the right to use, modify, copy and distribute
Software only when embedded on a Microchip microcontroller or digital signal
controller that is integrated into your product or third party product
(pursuant to the sublicense terms in the accompanying license agreement).

You should refer to the license agreement accompanying this Software for
additional information regarding your rights and obligations.

SOFTWARE AND DOCUMENTATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND,
EITHER EXPRESS OR IMPLIED, INCLUDING WITHOUT LIMITATION, ANY WARRANTY OF
MERCHANTABILITY, TITLE, NON-INFRINGEMENT AND FITNESS FOR A PARTICULAR PURPOSE.
IN NO EVENT SHALL MICROCHIP OR ITS LICENSORS BE LIABLE OR OBLIGATED UNDER
CONTRACT, NEGLIGENCE, STRICT LIABILITY, CONTRIBUTION, BREACH OF WARRANTY, OR
OTHER LEGAL EQUITABLE THEORY ANY DIRECT OR INDIRECT DAMAGES OR EXPENSES
INCLUDING BUT NOT LIMITED TO ANY INCIDENTAL, SPECIAL, INDIRECT, PUNITIVE OR
CONSEQUENTIAL DAMAGES, LOST PROFITS OR LOST DATA, COST OF PROCUREMENT OF
SUBSTITUTE GOODS, TECHNOLOGY, SERVICES, OR ANY CLAIMS BY THIRD PARTIES
(INCLUDING BUT NOT LIMITED TO ANY DEFENSE THEREOF), OR OTHER SIMILAR COSTS.
 *******************************************************************************/
// DOM-IGNORE-END


// *****************************************************************************
// *****************************************************************************
// Section: Included Files
// *****************************************************************************
// *****************************************************************************

#include <stddef.h>                     // Defines NULL
#include <stdbool.h>                    // Defines true
#include <stdlib.h>                     // Defines EXIT_FAILURE
#include "system/common/sys_module.h"   // SYS function prototypes
#include <xc.h>

//I have added the PWM to main
//i initialized the OC module as the program begins
// *****************************************************************************
// *****************************************************************************
// Section: Main Entry Point
// *****************************************************************************
// *****************************************************************************
void initOC(){
    //***output pin config for OC1 and OC2 for PWM function
    RPA0Rbits.RPA0R = 0b0101; // set OC1 to RPB7 (PIC pin # 16)
    RPA1Rbits.RPA1R = 0b0101; // set OC2 to RPB8 (PIC pin #  17)
    int sval = 0;
    // start T2 used for OC1 and OC2
    T3CONbits.TCKPS = 3;     // Timer2 PReSc N=16 0b100
	PR3 = 2999;              // period = (PR2+1) * N * 20.833 ns = 1000 us, 1 kHz
	TMR3 = 0;                // initialize at 0
	T3CONbits.ON = 1;        // turn on Timer2
	//Output compare at OC1
	OC1CONbits.OCM = 0b110;  	//PWM mode without fault pin; other OC1CON bits are defaults
	OC1CONbits.OC32 = 0;		//use the 16 bit timer
	OC1RS = sval;             	// duty cycle = OC1RS/(PR3+1) = 25%
	OC1R = sval;              	// initialize before turning OC1 on; afterward it is read-only
	OC1CONbits.OCTSEL = 1;  	//Use timer 2=0
	OC1CONbits.ON = 1;       	// turn on OC1
    //Output compare at OC2
	OC2CONbits.OCM = 0b110;  	//PWM mode without fault pin; other OC1CON bits are defaults
	OC2CONbits.OC32 = 0;		//use the 16 bit timer
	OC2RS = sval;             	// duty cycle = OC1RS/(PR3+1) = 25%
	OC2R = sval;              	// initialize before turning OC1 on; afterward it is read-only
	OC2CONbits.OCTSEL = 1;  	//Use timer 2=0
	OC2CONbits.ON = 1;       	// turn on OC2
    //SET Interrupt for TIMER2
//  	IPC2bits.T2IP = 5;            // interrupt priority default sub
//	IFS0bits.T2IF = 0;            // clear the int flag
//	IEC0bits.T2IE = 1;            // enable T2 as interrupt by setting IEC0
}



int main ( void )
{
    /* Initialize all MPLAB Harmony modules, including application(s). */
    SYS_Initialize ( NULL );
    TRISAbits.TRISA4 = 0;
    LATAbits.LATA4 = 0;
    initOC();
    OC1RS = 0;

    while ( true )
    {
        /* Maintain state machines of all polled MPLAB Harmony modules. */
        SYS_Tasks ( );

    }

    /* Execution should not come here during normal operation */

    return ( EXIT_FAILURE );
}


/*******************************************************************************
 End of File
*/

