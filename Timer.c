/*
 * File:   Timer.c
 * Author: John Santos and Anhela Francees
 *
 * Created on October 2nd, 2023, 6:47 PM
 */

#include "xc.h"
#include "Timer.h"
uint64_t set_clk;

// Timer 2/3 interrupt subroutine.
void __attribute__((interrupt, no_auto_psv))_T3Interrupt(void) {
    IFS0bits.T3IF = 0; // Clear timer 3 interrupt flag.
    uint16_t TMR3flag = 1; // Since T3IF may be auto cleared.
    T2CONbits.TON = 0; // Stop timer 3/2.
    return;
}

// Timer 2 interrupt subroutine.
void __attribute__((interrupt, no_auto_psv))_T2Interrupt(void) {
    IFS0bits.T2IF = 0; // Clear timer 2 interrupt flag.
    T2CONbits.TON = 0; // Stop timer 2.
    return;
}

void SetClk(uint16_t clk) {
    set_clk = clk;
    NewClk(clk); // Configure the clock.
}

/**
 * Configure Timer 2.
 */
void configure_timer_2() {
    T2CONbits.TCS = 0; // Use the internal clock fosc/2.
    T2CONbits.TSIDL = 0; // Continue the module operation at idle.
    T2CONbits.TCKPS = 0b00; // Use 1:1 scaling.
    T2CONbits.T32 = 0; // Timer2 and Timer3 act as two 16 bit timers.
    IPC1bits.T2IP = 0b111; // Set priority to level 7.
    IEC0bits.T2IE = 1; // Enable the interrupt.
    IFS0bits.T2IF = 0; // Clear the interrupt flag.
    return;
}

/**
 * Configure Timer 2 and 3 acting as one 32 bit timer. 
 * @param clk
 */
void configure_timer_2_3() {
    T2CONbits.TCS = 0; // Use internal clock fosc/2.
    T2CONbits.TSIDL = 0; // Continue the module operation at idle.
    T2CONbits.TCKPS = 0b00; // Use 1:1 scaling.
    T2CONbits.T32 = 1; // Timer2 and Timer3 form a single 32-bit timer.
    IPC1bits.T2IP = 0b111; // Set priority to level 7.
    IEC0bits.T3IE = 1; // Enable the interrupt.
    IFS0bits.T2IF = 0; // Clear the interrupt flag.
    return;
}

/**
 * Provides delay in seconds. 
 * @param time_sec The time in seconds to delay.
 */
void delay_sec(uint32_t time_sec) {
    T2CONbits.TON = 1; // Start the 32 bit timer 2/3.
    TMR2 = 0; // Clear timer 2.
    TMR3 = 0; // Clear timer 3.
    // An interrupt is generated when PR3:PR2 = TMR3:TMR2
    uint32_t pr23 = calculate_pr_2_3(time_sec); // Compute PR2 (16 bits) and PR3 (16 bits). 
    // Implement bit shifting for 32 bits.
    PR2 = pr23; // Least significant word.
    PR3 = pr23 >> 16; // Most significant word.
    Idle();
    return;   
}

/*
 * Provides delay in milliseconds.
 * @param: time_ms The time in millseconds to delay.
 * @param idle_on: Set to 1 to enable idle.
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
 * Provides a delay in microseconds.
 * @param time_us: The time in microseconds to delay.
 * @param idle_on: Set to 1 to enable idle.
 */
void delay_us(uint16_t time_us, uint16_t idle_on) {
    T2CONbits.TON = 1; // Start the 16 bit timer 2.
    TMR2 = 0; // Clearing timer 2.
    // This just assumes that using this delay passes an 8MHz clock. 
    PR2 = (uint32_t) (time_us * set_clk)/2;
    if (idle_on == 1) {
        Idle();
    }
    return;
}

/**
 * Calculates the 32 bit PR2:3 to provide second delays.
 * @param time: The time in seconds to delay.
 * @return: The 32 bit PR2:3.
 */
uint32_t calculate_pr_2_3(uint32_t time) {
    if (set_clk == 8) { 
        return (uint32_t) (time * set_clk*1E6)/2;
    } 
    return (uint32_t) (time * set_clk*1E3)/2;   
}

/**
 * Calculates the 16 bit PR2.
 * @param time: The time in seconds to delay.
 * @return: PR2
 */
uint16_t calculate_pr_2(float time) {
    if (set_clk == 8) { 
        return (uint16_t) (time * set_clk*1E6)/2;
    } 
    return (uint16_t) (time * set_clk*1E3)/2;   
}