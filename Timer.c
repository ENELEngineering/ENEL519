/*
 * File:   Timer.c
 * Author: johns
 *
 * Created on November 4, 2023, 10:35 PM
 */

#include "xc.h"
#include "Timer.h"
#include "ChangeClk.h"
uint16_t set_clk;

void SetClk(uint16_t clk) {
    NewClk(clk);
    set_clk = clk;
    return;
}

/**
 * Timer 1 Interrupt Subroutine
 */
void __attribute__((interrupt, no_auto_psv))_T1Interrupt(void) {
    IFS0bits.T1IF = 0; // Clear timer 1 interrupt flag.
    T1CONbits.TON = 0; // Stop timer 1.
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
 * Timer 3 interrupt subroutine.
 */
void __attribute__((interrupt, no_auto_psv))_T3Interrupt(void) {
    IFS0bits.T3IF = 0; // Clear timer 3 interrupt flag.
    T3CONbits.TON = 0; // Stop timer 3.
    return;
}

/**
 * Timer 1 configuration.
 * @param clk: The set system clock.
 */
void configure_timer_1() {
    T1CONbits.TCS = 0; // Use the internal clock fosc/2.
    T1CONbits.TSIDL = 0; // Continue module operation at idle.
    T2CONbits.TCKPS = 0b00; // Clock prescaler uses 1:1 scaling.
    IPC0bits.T1IP = 0b111; // Priority level is 7.
    IEC0bits.T1IE = 1; // Enable the interrupt.
    IFS0bits.T1IF = 0; // Clear the interrupt flag.
    return;
}

/**
 * Timer 2 configuration.
 * @param clk: The set system clock.
 */
void configure_timer_2() {
    T2CONbits.TCS = 0; // Use the internal clock fosc/2.
    T2CONbits.TSIDL = 0; // Continue module operation at idle.
    T2CONbits.TCKPS = 0b00; // Clock prescaler uses 1:1 scaling.
    T2CONbits.T32 = 0; // Timer2 and Timer3 act as two 16 bit timers.
    IPC1bits.T2IP = 0b111; // Priority level is 7.
    IEC0bits.T2IE = 1; // Enable the interrupt.
    IFS0bits.T2IF = 0; // Clear the interrupt flag.
    return;
}

/**
 * Timer 3 configuration.
 * @param clk: The set system clock.
 */
void configure_timer_3() {
    T3CONbits.TCS = 0; // Use the internal clock fosc/2.
    T3CONbits.TSIDL = 0; // Continue module operation at idle.
    T3CONbits.TCKPS = 0b11; // Clock prescaler uses 1:256 scaling.
    IPC2bits.T3IP = 0b111; // Priority level is 7.
    IEC0bits.T3IE = 1; // Enable the interrupt.
    IFS0bits.T3IF = 0; // Clear the interrupt flag;
    return;
}

/**
 * Provide a delay in microseconds.
 * @param time_us: The microsecond delay to perform (uint16_t)
 * @param idle_on: Set to 1 to idle.
 */
void delay_us(uint16_t time_us, uint16_t idle_on) {
    T1CONbits.TON = 1; // Start the 16 bit timer 1.
    TMR1 = 0; // Clear timer 1 at the start.
    PR1 = (uint16_t) (time_us * set_clk)/2; // Assumes using an 8MHz clock at 1:1 scaling.
    
    if (idle_on == 1){
        Idle();
    }
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
    PR2 = (uint16_t) (time_ms * set_clk)/2; // Assumes using either 500KHz or 32KHz clock and 1:1 scaling.
    
    if (idle_on == 1) {
        Idle();
    }
    return;
}
