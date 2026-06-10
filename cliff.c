/*
 * adc.h
 *
 *  Created on: Mar 22, 2022
 *      Author: syukri
 */

#ifndef ADC_H_
#define ADC_H_

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>
#include "Timer.h"
#include <inc/tm4c123gh6pm.h>
#include "lcd.h"
#include "driverlib/interrupt.h"


void adc_init(void);
uint16_t adc_read(void);
double raw_to_dist(double value);


#endif /* ADC_H_ */
