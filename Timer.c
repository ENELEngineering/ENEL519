/*
 * File:   Timer.c
 * Author: johns
 *
 * Created on September 25, 2023, 6:47 PM
 */

#include "xc.h"

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
     
    // T2CON Configuration... Start the 16 bit Timer2.
    T2CONbits.TON = 1;
    
    // Compute PR2
    PR2 = calculate_pr2(time_ms, 32);
    
    Idle();
    return;
}

void delay_us(uint16_t time_us, uint16_t idle_on) {
    uint16_t clk = 8;
    // Configure the clock
    NewClk(clk); 
    // T2CON Configuration... Start the timer.
    T2CONbits.TON = 1;
    T2CONbits.TCS = 0;
    T2CONbits.TSIDL = 0;
    T2CONbits.TCKPS = 0b00;
    T2CONbits.T32 = 0;
   
    // Compute PR2
    PR2 = calculate_pr2(time_us, clk);
    
    //while(TMR2 == PR2);
    
    // Timer 2 Interrupt Configuration
    // Set priority to level 7.
    IPC1bits.T2IP = 0b111;
    // Enable the interrupt.
    IEC0bits.T2IE = 1;
    // Set the interrupt flag.
    IFS0bits.T2IF = 1;     
     
    Idle();
    return;
}

int calculate_pr2(uint16_t time, uint16_t clk) {
    uint32_t pr2 = (time * clk)/2;
    return pr2;
}


void configure_timer_2(void) {
//    uint16_t clk = 32;
//    // Configure the clock
//    NewClk(clk);
    
    // Configure the clock source -> 0 is the internal clock fosc/2.
    T2CONbits.TCS = 0;
    // Setting to 0 means to continue the module operation at idle.
    T2CONbits.TSIDL = 0;
    // Clock prescale select bits 00 -> 1:1 scaling.
    T2CONbits.TCKPS = 0b00;
    // Setting to 0 means Timer2 and Timer3 act as two 16 bit timers.
    T2CONbits.T32 = 0;
    
    //T2CON = 0b0000001000000001;
    // Timer 2 Interrupt Configuration
    // Set priority to level 7.
    IPC1bits.T2IP = 0b111;
    // Enable the interrupt.
    IEC0bits.T2IE = 1;
    // Clear the interrupt flag.
    IFS0bits.T2IF = 0;
    
}


