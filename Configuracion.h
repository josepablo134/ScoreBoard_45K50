#ifndef CONFIGURACION_H
#define	CONFIGURACON_H

#include <xc.h>
#include <pic18f45k50.h>

/*
 *
 * CONFIGURACION DE PERIFERICOS
 * 
 * */

#pragma config WDTEN    = OFF
#pragma config PBADEN   = OFF   //PORTB AS DIGITAL I/O
#pragma config CFGPLLEN = OFF   //PLL Disabled
#pragma config FOSC     = INTOSCIO//Internal osc

#pragma config BORV     = 190

#pragma config DEBUG    = OFF
#pragma config LVP      = ON
#pragma config MCLRE    = OFF

#pragma config CP0      = OFF, CP1 = OFF, CP2 = OFF, CP3 = OFF
#pragma config CPB      = OFF                                                                              // CPB off
#pragma config CPD      = OFF
#pragma config WRT0     = OFF, WRT1 = OFF, WRT2 = OFF, WRT3 = OFF
#pragma config WRTC     = OFF
#pragma config WRTB     = OFF
#pragma config WRTD     = OFF
#pragma config EBTR0    = OFF, EBTR1 = OFF, EBTR2 = OFF, EBTR3 = OFF
#pragma config EBTRB    = OFF

#define _XTAL_FREQ  8000000



#endif

