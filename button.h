/*
 * adc.c
 *
 *  Created on: Mar 22, 2022
 *      Author: syukri
 */
#include <inc/tm4c123gh6pm.h>
#include <stdint.h>
#include "adc.h"
#include <stdbool.h>
#include "driverlib/interrupt.h"


void adc_init (void)
{
    SYSCTL_RCGCADC_R |= 0x001;    // 1) activate ADC0
    SYSCTL_RCGCGPIO_R |= 0x02;    // 2) activate clock for Port B

    while((SYSCTL_PRGPIO_R&0x02) != 0x02){};  // 3 for stabilization

    GPIO_PORTB_DIR_R &= ~0x10;    // 4) make PB4 input

    GPIO_PORTB_AFSEL_R |= 0x10;   // 5) enable alternate function on PB4
    GPIO_PORTB_DEN_R &= ~0x10;    // 6) disable digital I/O on PB4
    GPIO_PORTB_AMSEL_R |= 0x10;   // 7) enable analog functionality on PB4

    ADC0_PC_R &= ~0xF;
    ADC0_PC_R |= 0x1;             // 8) configure for 125K samples/sec

    ADC0_ACTSS_R &= 0xFFFE;      // 10) disable sample sequencer 0 page 821

    ADC0_EMUX_R &= 0xFFF0;       // 11) seq0 is software trigger


    ADC0_SSMUX0_R |= 0x000A;           // 12) set first sample to analog input channel AIN10, PB4 (page 851)

    ADC0_SSCTL0_R = (0b100 | 0b010);       // 13) yes IE0 END0 (enable raw status) page 853
    ADC0_IM_R &= ~0x0001;         // 14) disable SS0 interrupts
    ADC0_ACTSS_R |= 0x0001;       // 15) activate sample sequencer 0
}



//------------ADC0_InSeq3------------

// Busy-wait analog to digital conversion

// Input: none

// Output: 12-bit result of ADC conversion

uint16_t adc_read(void)
{

    uint16_t result;

    ADC0_PSSI_R = 0x0001;            // 1) initiate SS0 - start ADC conversion page 846

    while((ADC0_RIS_R&0x01)==0){};   // 2) wait for conversion done in SS0

    result = ADC0_SSFIFO0_R & 0xFFF;   // 3) read return SS0 result

    ADC0_ISC_R = 0x01;             // 4) acknowledge completion

    result =raw_to_dist(result);

    return result;
}

double raw_to_dist(double value)
{
    double calc;
    calc = (12387928*pow(value,-1.8036));
    return calc;

}

double adc_to_cm (double adc) {
    return pow(adc, -1.347) * 434505;
}
