/*
 * File:   Timer.c
 * Author: John Santos and Anhela Francees
 *
 * Created on October 2nd, 2023, 6:47 PM
 */

#include "xc.h"
#include "Timer.h"
uint64_t set_clk;

/**
 * Configure the system clock
 * @param clk Can be 8MHz, 32KHz, or 500KHz.
 */
void SetClk(uint16_t clk) {
    set_clk = clk;
    NewClk(clk);
    return;
}

// Timer 2 interrupt subroutine.
void __attribute__((interrupt, no_auto_psv))_T2Interrupt(void) {
    IFS0bits.T2IF = 0; // Clear timer 2 interrupt flag.
    T2CONbits.TON = 0; // Stop timer
    return;
}

/**
 * T2CON Configuration...
 * @param clk
 */
void configure_timer_2(void) {
    T2CONbits.TCS = 0; // Use the internal clock source fosc/2.
    T2CONbits.TSIDL = 0; // Continue the module operation at idle.
    T2CONbits.TCKPS = 0b00; // Use 1:1 scaling.
    T2CONbits.T32 = 0; // Timer2 and Timer3 act as two 16 bit timers.
    IPC1bits.T2IP = 0b111; // Set priority to level 7.
    IEC0bits.T2IE = 1; // Enable the interrupt.
    IFS0bits.T2IF = 0; // Enable the interrupt.
    return;
}

/*
 * This function provides a millsecond delay.
 * @param: interval is the time in ms to delay.
 * @param idle_on Set to 1 to enable idle.
 */
void delay_ms(uint16_t time_ms, uint16_t idle_on) {
    T2CONbits.TON = 1; // Start the 16 bit timer 2.
    TMR2 = 0; // Clearing Timer 2.
    PR2 = calculate_pr_2(time_ms*1e-3); // Compute PR2...convert time_ms to seconds.
    if (idle_on == 1) {
        Idle();
    }
    return;
}

/**
 * This function provides a delay in microseconds.
 * @param time_us The microsecond time to delay. 
 * @param idle_on Set to 1 to allow idle.
 */
void delay_us(uint16_t time_us, uint16_t idle_on) {
    T2CONbits.TON = 1; // Start the 16 bit Timer2.
    TMR2 = 0; // Clearing Timer 2.
    // This computation assumes that using this delay passes an 8MHz clock. 
    PR2 = (uint32_t) (time_us * set_clk)/2;
    if (idle_on == 1) {
        Idle();
    }
    return;
}

/**
 * This function calculate PR2 needed for the delay.
 * @param time This is the time converted to seconds.
 * @return The PR2 integer.
 */
uint16_t calculate_pr_2(float time) {
    if (set_clk == 8) { 
        return (uint16_t) (time * set_clk*1E6)/2;
    } 
    return (uint16_t) (time * set_clk*1E3)/2;   
}