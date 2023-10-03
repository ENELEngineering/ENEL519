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
    // Clear timer 2 interrupt flag - auto cleared in some PICs.
    IFS0bits.T3IF = 0; 
    // Set user defined global variable since T3IF may be auto cleared.
    uint16_t TMR3flag = 1;
    // Stop timer 3/2.
    T2CONbits.TON = 0;
    return;
}

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

void delay_sec(uint32_t time_sec) {
    // Start the 32 bit Timer2/3.
    T2CONbits.TON = 1;
    // Clearing Timer 2 and 3.
    TMR2 = 0; // Least significant word
    TMR3 = 0; // Most significant word
    
    // Compute PR2 (16 bits) and PR3 (16 bits). Implement bit shifting for 32 bits.
    // An interrupt is generated when PR3:PR2 = TMR3:TMR2
    uint32_t pr23 = calculate_pr_2_3(time_sec);
    PR2 = pr23; // Least significant word.
    PR3 = pr23 >> 16; // Most significant word.
    
    Idle();
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

uint32_t calculate_pr_2_3(uint32_t time) {
    if (set_clk == 8) { 
        return (uint32_t) (time * set_clk*1E6)/2;
    } 
    return (uint32_t) (time * set_clk*1E3)/2;   
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

void configure_timer_2_3(uint16_t clk) {
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
    // Setting to 1 means Timer2 and Timer3 form a single 32-bit timer.
    T2CONbits.T32 = 1;
    
    // Timer 2/3 Interrupt Operation
    // Set priority to level 7.
    IPC1bits.T2IP = 0b111;
    // Enable the interrupt.
    IEC0bits.T3IE = 1;
    // Clear the interrupt flag.
    IFS0bits.T2IF = 0;
    return;
}

