/*
 * File:   Timer.c
 * Author: John Santos and Anhela Francees
 *
 * Created on October 2nd, 2023, 6:47 PM
 */

#include "xc.h"
#include "Timer.h"
uint64_t set_clk;

void SetClk(uint16_t clk) {
    NewClk(clk);
    set_clk = clk;
    return;
}

/**
 * Timer 2 interrupt subroutine.
 */
void __attribute__((interrupt, no_auto_psv))_T2Interrupt(void) {
    IFS0bits.T2IF = 0; // Clear timer 2 interrupt flag.
    T2CONbits.TON = 0; // Stop timer 2.
    return;
}

/**
 * Timer 2 configuration.
 * @param clk: The set system clock.
 */
void configure_timer_2() {
    T2CONbits.TCS = 0; // Use the internal clock fosc/2.
    T2CONbits.TSIDL = 0; // Continue module operation at idle.
    T2CONbits.TCKPS = 0b10; // Clock prescaler uses 1:64 scaling.
    T2CONbits.T32 = 0; // Timer2 and Timer3 act as two 16 bit timers.
    IPC1bits.T2IP = 0b111; // Priority level is 7.
    IEC0bits.T2IE = 1; // Enable the interrupt.
    IFS0bits.T2IF = 0; // Clear the interrupt flag.
    return;
}

void delay_sec(uint16_t time_sec) {
    T2CONbits.TON = 1;
    TMR2 = 0; // Least significant word
    PR2 = (uint16_t) (set_clk * time_sec)/(2*64);  
    return;
}

/*
 * Provide a delay in milliseconds.
 * @param time_ms: The millisecond delay to perform (uint16_t).
 * @param idle_on: Set to 1 to idle.
 */
void delay_ms(uint16_t time_ms, uint16_t idle_on) {
    T2CONbits.TON = 1; // Start the 16 bit timer 2.
    TMR2 = 0; // Clear timer 2 at the start.
    PR2 = (uint16_t) (time_ms * set_clk * 1000)/2;
    
    if (idle_on == 1) {
        Idle();
    }
    return;
}


