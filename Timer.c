/*
 * File:   Timer.c
 * Author: John Santos and Anhela Francees
 *
 * Created on October 2nd, 2023, 6:47 PM
 */

#include "xc.h"
#include "Timer.h"
uint64_t set_clk;

// Time 2 interrupt subroutine.
void __attribute__((interrupt, no_auto_psv))_T2Interrupt(void) {
    // Clear timer 2 interrupt flag
    IFS0bits.T2IF = 0;
    // Stop timer
    T2CONbits.TON = 0;
    // Global variable created by user
    //TMR2flag = 1;
    return;
}

/*
 * This function blinks the LED at a set time. 
 * @param: interval is the time in ms to blink the LED.
 */
void delay_ms(uint16_t time_ms, uint16_t idle_on) {
    // Start the 16 bit Timer2.
    T2CONbits.TON = 1;
    // Clearing Timer 2.
    TMR2 = 0;

    // Compute PR2...convert time_ms to seconds.
    PR2 = calculate_pr_2(time_ms*1e-3);
    
    Idle();
    return;
}

void delay_us(uint16_t time_us, uint16_t idle_on) {
    // Start the 16 bit Timer2.
    T2CONbits.TON = 1;
    // Clearing Timer 2.
    TMR2 = 0;
    
    // Compute PR2...convert time_us to seconds.
    //PR2 = calculate_pr_2(time_us*1e-6);
    
    // This just assumes that using this delay passes an 8MHz clock. 
    // Other methods for convert clock to Hz fails...
    PR2 = (uint32_t) (time_us * set_clk)/2;
    
    Idle();
    return;
}

uint16_t calculate_pr_2(float time) {
    if (set_clk == 8) { 
        return (uint16_t) (time * set_clk*1E6)/2;
    } 
    return (uint16_t) (time * set_clk*1E3)/2;   
}

void configure_timer_2(uint16_t clk) {
    set_clk = clk;
    // Configure the clock
    NewClk(clk);
    
    // T2CON Configuration...
    // Configure the clock source -> 0 is the internal clock fosc/2.
    T2CONbits.TCS = 0;
    // Setting to 0 means to continue the module operation at idle.
    T2CONbits.TSIDL = 0;
    // Clock prescale select bits 00 -> 1:1 scaling.
    T2CONbits.TCKPS = 0b00;
    // Setting to 0 means Timer2 and Timer3 act as two 16 bit timers.
    T2CONbits.T32 = 0;
    
    // Timer 2 Interrupt Operation
    // Set priority to level 7.
    IPC1bits.T2IP = 0b111;
    // Enable the interrupt.
    IEC0bits.T2IE = 1;
    // Clear the interrupt flag.
    IFS0bits.T3IF = 0;
    return;
}