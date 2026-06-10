
#include "ping.h"
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
volatile unsigned int rising_time;
volatile unsigned int falling_time = 0;
volatile enum
{
    LOW, HIGH, DONE
} state;
uint32_t period;
uint32_t edgeCount;
uint32_t count = 0;
int intFlag = 1;
int over_flow = 0;
void send_pulse()
{
    //send pulse to sensor listen for ping response
    TIMER3_CTL_R &= ~0x100;
    TIMER3_IMR_R &= ~0x400;
    GPIO_PORTB_AFSEL_R &= ~0b00001000;
    GPIO_PORTB_DIR_R |= 0b00001000;
    GPIO_PORTB_DATA_R &= ~0b00001000;
    GPIO_PORTB_DATA_R |= 0b00001000;
    timer_waitMicros(5);
    GPIO_PORTB_DATA_R &= ~0b00001000;
    TIMER3_ICR_R |= 0x400;
    GPIO_PORTB_AFSEL_R |= 0b00001000;
    TIMER3_CTL_R |= 0x100;
    TIMER3_IMR_R |= 0x400;

}
void ping_init()
{
    //initialize registers
    SYSCTL_RCGCGPIO_R |= 0b00000010;
    SYSCTL_RCGCTIMER_R |= 0b00001000;
    GPIO_PORTB_DEN_R |= 0b00001000;
    GPIO_PORTB_AFSEL_R |= 0b00001000;
    GPIO_PORTB_PCTL_R |= ((0x00007000) & (0xFFF0FFFF));
    TIMER3_CTL_R &= ~0x100;
    TIMER3_CFG_R |= 0x4;
    TIMER3_TBILR_R |= 0xFFFF;
    TIMER3_TBMR_R |= 0x0007;
    TIMER3_TBMR_R &= ~0x0010;
    TIMER3_TBPR_R |= 0xFF;
    TIMER3_CTL_R |= 0xC00;
    TIMER3_ICR_R |= 0x400;
    TIMER3_IMR_R |= 0x400;
    TIMER3_CTL_R |= 0x100;
    NVIC_EN1_R |= 0x10;
    NVIC_PRI9_R &= (0xFFFFFF0F | 0x20);
    IntRegister(INT_TIMER3B, TIMER3B_Handler);
    IntMasterEnable();

}
void TIMER3B_Handler()
{
    //store timer values when interrupts are triggered
    if (TIMER3_MIS_R & 0x400)
    {
        TIMER3_ICR_R |= 0x400;

        if (state == LOW)
        {
            rising_time = TIMER3_TBR_R;
            state = HIGH;
        }
        else if (state == HIGH)
        {
            falling_time = TIMER3_TBR_R;
            state = DONE;
        }

    }

}
unsigned int ping_read()
{
    //send signal to sensor, wait for isr to collect rising
    //and falling edges and return the pulse width
    unsigned int time;
    send_pulse();
    while (state != DONE)
    {
    }
    if (falling_time > rising_time)
    {
        rising_time = (rising_time - falling_time) + (1 << 24);
        over_flow++;
    }
    time = rising_time - falling_time;
    state = LOW;
    return time;
}
int overflow(){
    return over_flow;
}
