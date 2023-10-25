/*
 * File:   TVIR.c
 * Author: johns
 *
 * Created on October 20, 2023, 4:34 PM
 */

#include "xc.h"
#include "TVIR.h"
#include "Timer.h"

// Global Variables
uint16_t RB4_Flag, RA2_Flag;
uint16_t ninterrupt=0;

// Configured 0 as Channel Mode and 1 as Volume Mode.
uint16_t mode=0;

/* Create the ISR routine. Common interrupt routine for all CN inputs.
 * The interrupt is triggered by any change of state: hi to lo, lo to hi.
 * Interrupts will be triggered for debounces on push buttons too.
 * Clear the interrupt flag.
 * In interrupt routine, check the PORTs to determine 
 * which IO pin had a change of notification.
 */
void __attribute__((interrupt, no_auto_psv))_CNInterrupt(void) {
    IEC1bits.CNIE = 0; // Handle debounce effects by disabling the interrupts.
    delay_us(10, 1); // Allow debounces to settle by setting a delay.
    Nop();
    
    if (IFS1bits.CNIF = 1) {
        ninterrupt += 1;
    }
    
    // Set flags depending on which push buttons are pressed.
    if (PORTBbits.RB4 == 0 && PORTAbits.RA2 == 0) {
        RB4_Flag = 1;
        RA2_Flag = 1;
    }
    else if (PORTBbits.RB4 == 0 && PORTAbits.RA2 == 1) {
        RB4_Flag = 1;
        RA2_Flag = 0;
    }
    else if (PORTBbits.RB4 == 1 && PORTAbits.RA2 == 0) {
        RB4_Flag = 0;
        RA2_Flag = 1;
    }
    else if (PORTBbits.RB4 = 1 && PORTAbits.RA2 == 1) {
        RB4_Flag = 0;
        RA2_Flag = 0;
    }
    else{
        RB4_Flag = 0;       
        RA2_Flag = 0;
    }
    IFS1bits.CNIF = 0; // Clear the IF flag.
    IEC1bits.CNIE = 1; // Re-enable the interrupt.
}

/* 
 * Function to initialize all the I/O ports and the CN interrupts.
 */
void CN_init(){ 
    AD1PCFG = 0xFFFF; // Turn all analog pins as digital on RA2
    TRISAbits.TRISA2 = 1; // Set RA2 as input (Top button).
    TRISBbits.TRISB4 = 1; // Set RB4 as input (Buttom button).
    CNPU2bits.CN30PUE = 1; // Set pull up on RA2.
    CNPU1bits.CN1PUE = 1; // Set pull up on RB4.
    CNEN2bits.CN30IE = 1; // Enable the interrupt for RA2.
    CNEN1bits.CN1IE = 1; // Enable the interrupt for RB4.
    IPC4bits.CNIP = 0b111; // Set highest priority for interrupt.
    IEC1bits.CNIE = 1; // Interrupt request enabled.
    return;
}

/**
 * Check push button behaviour.
 * Power On/Off => Both buttons are pressed for more than 3 seconds.
 * Channel/Volume Mode switch => Both buttons are pressed for less than 3 seconds.
 * Channel/Volume Up => RA2 pressed.
 * Channel/Volume Down => RB4 pressed. 
 */
void CN_check() {  
    // Detect both push buttons are pressed.
    if (RB4_Flag == 1 && RA2_Flag == 1) {
        T3CONbits.TON = 1; // Start the 16 bit timer 3.
        TMR3 = 0; // Clear timer 3 at the start.
        Disp2String("\n\r CN1/RB4, CN30/RA2 are pressed\n");
    } 
    
    // Detect both push buttons are released.
    if (RB4_Flag == 0 && RA2_Flag == 0) {
        if (T3CONbits.TON == 1) {
            T3CONbits.TON == 0; // Stop the 16 bit timer 3.
            // 46875 translates to 3 seconds at 8MHz clock with 256 prescaler.
            if (TMR3 >= 46875) {
                Disp2String("\n\r Power ON/OFF\n");
            } else {
                mode = !mode;
                if (mode == 1) {
                    Disp2String("\n\r Selected Volume Mode\n");       
                } else {
                    Disp2String("\n\r Selected Channel Mode\n");
                } 
            }
        }
    }
    // Detect top button RA2 is pressed.
    else if (RB4_Flag == 0 && RA2_Flag == 1) {
        if (mode == 1) {
            Disp2String("\n\r Volume Up\n");       
        } else {
            Disp2String("\n\r Channel Up\n");       
        }
    }
    // Detect buttom button RB4 is pressed.
    else if (RB4_Flag == 1 && RA2_Flag == 0) {
        if (mode == 1) {
            Disp2String("\n\r Volume Down\n");       
        } else {
            Disp2String("\n\r Channel Down\n");       
        }
    }
    
    RB4_Flag = 0; 
    RA2_Flag = 0;
    
    if (ninterrupt >= 2) {
        ninterrupt = 0;
    }
}

/**
 * Carrier signal is created with a 38KHz frequency which means a period of
 * 26.32us, thus on bit of 13.16us and off bit of 13.16us.
 */
void carrier_signal() {
    LATBbits.LATB9 = 1;
    delay_us(13, 1);
    LATBbits.LATB9 = 0;
    delay_us(13, 1);
    return;
}

void start_bit_signal() {
    T2CONbits.TON = 1; // Start the 16 bit timer 2.
    TMR2 = 0; // Clear timer 2 at the start.
    // 18000 represents 4.5ms on a 8MHz clock with 1:1 prescaler.
    while (TMR2 <= 18000) {
        carrier_signal();
    }
    T2CONbits.TON == 0; // Stop the 16 bit timer 3.
    
    T2CONbits.TON = 1; // Start the 16 bit timer 2.
    TMR2 = 0; // Clear timer 2 at the start.
    while(TMR2 <= 18000) {
        LATBbits.LATB9 = 0;
    }
    T2CONbits.TON == 0; // Stop the 16 bit timer 3.
    return;
}

void one_bit_signal() {
    T2CONbits.TON = 1; // Start the 16 bit timer 2.
    TMR2 = 0; // Clear timer 2 at the start.
    // 2240 represents 0.56ms on a 8MHz clock with 1:1 prescaler.
    while (TMR2 <= 2240) {
        carrier_signal();
    }
    T2CONbits.TON == 0; // Stop the 16 bit timer 3.
    
    T2CONbits.TON = 1; // Start the 16 bit timer 2.
    TMR2 = 0; // Clear timer 2 at the start.
    // 6760 represents 1.69ms on a 8MHz clock with 1:1 prescaler.
    while(TMR2 <= 6760) {
        LATBbits.LATB9 = 0;
    }
    T2CONbits.TON == 0; // Stop the 16 bit timer 3.
    return;
}

void zero_bit_signal() {
    T2CONbits.TON = 1; // Start the 16 bit timer 2.
    TMR2 = 0; // Clear timer 2 at the start.
    // 2240 represents 0.56ms on a 8MHz clock with 1:1 prescaler.
    while (TMR2 <= 2240) {
        carrier_signal();
    }
    T2CONbits.TON == 0; // Stop the 16 bit timer 3.
    
    T2CONbits.TON = 1; // Start the 16 bit timer 2.
    TMR2 = 0; // Clear timer 2 at the start.
    while(TMR2 <= 2240) {
        LATBbits.LATB9 = 0;
    }
    T2CONbits.TON == 0; // Stop the 16 bit timer 3.
    return;
}

void _power_on_off() {
    return;
}

void _volume_up_() {
    return;
}

void _volume_down() {
    return;
}

void _channel_up() {
    return;
}

void _channel_down() {
    return;
}