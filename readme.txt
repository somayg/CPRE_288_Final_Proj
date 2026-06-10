//#ifndef PING_H_
//#define PING_H_
//
//#include <stdint.h>
//#include <stdbool.h>
//#include <inc/tm4c123gh6pm.h>
//#include "driverlib/interrupt.h"
//
//void ping_init(void);
//
//void ping_trigger(void);
//
//void TIMER3B_Handler(void);
//
//float ping_getDistance(void);
//
//#endif /* PING_H_ */


#ifndef PING_H_
#define PING_H_
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <inc/tm4c123gh6pm.h>
#include "Timer.h"
#include <stdbool.h>
#include <driverlib/interrupt.h>


void ping_init(void);
void send_pulse(void);
void TIMER3B_Handler(void);
unsigned int ping_read(void);
int overflow(void);
#endif /* PING_H_ */
