/*
 * File:   IO.c
 * Author: johns
 *
 * Created on October 2, 2023, 2:48 PM
 */

#include "xc.h"
#include "IO.h"
#include "Timer.h"

/* Create the ISR routine. Common interrupt routine for all CN inputs.
 * The interrupt is triggered by any change of state: hi to lo, lo to hi.
 * Interrupts will be triggered for debounces on push buttons too.
 * Clear the interrupt flag
 * In interrupt routine, check the PORTs to determine 
 * which IO pin had a change of notification.
 */

// Global Variables
volatile uint16_t old_RB4_status, old_RA4_status, old_RA2_status;
uint16_t SINGLE_PRESS_Flag, MULTIPLE_PRESS_Flag;

void __attribute__((interrupt, no_auto_psv))_CNInterrupt(void) {
     
    if (IFS1bits.CNIF == 1) {
        // Should handle debounce effects.
        delay_ms(150, 1);
        if (PORTBbits.RB4 == 0) {
            // If RB4 is interrupting
            if ((old_RA4_status == 1 && PORTAbits.RA4 == 0) || (old_RA2_status == 1 && PORTAbits.RA2 == 0)) {
                MULTIPLE_PRESS_Flag = 1;   
                SINGLE_PRESS_Flag = 0;
            } else if (old_RB4_status == 1) {
                MULTIPLE_PRESS_Flag = 0;
                SINGLE_PRESS_Flag = 1;
            } else {
                MULTIPLE_PRESS_Flag = 0;
                SINGLE_PRESS_Flag = 0;
            }
        } 
        else if (PORTAbits.RA4 == 0) {
            if ((old_RB4_status == 1 && PORTBbits.RB4 == 0) || (old_RA2_status == 1 && PORTAbits.RA2 == 0)) {
                MULTIPLE_PRESS_Flag = 1;
                SINGLE_PRESS_Flag = 0;
            } else if (old_RA4_status == 1) {
                MULTIPLE_PRESS_Flag = 0;
                SINGLE_PRESS_Flag = 1;
            } else {
                MULTIPLE_PRESS_Flag = 0;
                SINGLE_PRESS_Flag = 0;
            }
        } 
        else if (PORTAbits.RA2 == 0) {
            if ((old_RB4_status == 1 && PORTBbits.RB4 == 0) || (old_RA4_status == 1 && PORTAbits.RA4 == 0)) {
                MULTIPLE_PRESS_Flag = 1;
                SINGLE_PRESS_Flag = 0;
            } else if (old_RA2_status == 1) {
                MULTIPLE_PRESS_Flag = 0;
                SINGLE_PRESS_Flag = 1;
            } else {
                MULTIPLE_PRESS_Flag = 0;
                SINGLE_PRESS_Flag = 0;
            }
        }
        
        old_RB4_status = PORTBbits.RB4;
        old_RA4_status = PORTAbits.RA4;
        old_RA2_status = PORTAbits.RA2;
        
        if (MULTIPLE_PRESS_Flag == 1){
            display_message();
        } 
        else {
            if (SINGLE_PRESS_Flag == 1) {
                display_single_message();
            }
        }
    }
    
    // Clear the IF flag.
    IFS1bits.CNIF = 0;
    
    MULTIPLE_PRESS_Flag = 0;
    SINGLE_PRESS_Flag = 0;
    Nop();
}

void display_single_message(void) {
    if (PORTBbits.RB4 == 0) {
        Disp2String("\n\r CN1/RB4 is pressed\n");
    }
    else if (PORTAbits.RA4 == 0) {
        Disp2String("\n\r CN0/RA4 is pressed\n");   
    }
    else if (PORTAbits.RA2 == 0) {
        Disp2String("\n\r CN30/RA2 is pressed\n");
    }
}

void display_message(void) {
    if (PORTBbits.RB4 == 0 && PORTAbits.RA4 == 0 && PORTAbits.RA2 == 0) {
        Disp2String("\n\r CN1/RB4, CN0/RA4, CN30/RA2 are pressed\n");
    }
    else if (PORTBbits.RB4 == 0 && PORTAbits.RA4 == 0 && PORTAbits.RA2 == 1) {
        Disp2String("\n\r CN1/RB4, CN0/RA4 are pressed\n");
    }
    else if (PORTBbits.RB4 == 0 && PORTAbits.RA4 == 1 && PORTAbits.RA2 == 0) {
        Disp2String("\n\r CN1/RB4, CN30/RA2 are pressed\n");
    }
    else if (PORTBbits.RB4 == 1 && PORTAbits.RA4 == 0 && PORTAbits.RA2 == 0) {
        Disp2String("\n\r CN0/RA4, CN30/RA2 are pressed\n");
    }
}

/* Peripheral Configuration
 * Configure IO pins as input: TRISxBITS.TRISx# = 1;
 * Configure IO pin to be either pulled up or pulled low using 
 * internal resistors in PIC.
 */
void configure_peripherals(void) {
    // Configure push button pins (RA2, RA4, RB4) as inputs.
    TRISAbits.TRISA2 = 1;
    TRISAbits.TRISA4 = 1;
    TRISBbits.TRISB4 = 1;

    // Enable Pull-up on the push button pins with register.
    CNPU2bits.CN30PUE = 1;
    CNPU1bits.CN0PUE = 1;
    CNPU1bits.CN1PUE = 1;
}

/* Wakes up the PIC from idle or sleep when push buttons tied to
 * RB4/CN1, RA4/CN0, and RA2/CN30 are pushed.
 * Displays the status of the push buttons on Teraterm window when 
 * one of more buttons are pushed. "CN1/RB4 is pressed", 
 * "CN1/RB4 and CN0/RA4 are pressed."
 */
void CN_init(void) {
    // Set the interrupt priority to highest priority.
    IPC4bits.CNIP = 0b001;

    // To handle debounce effects, disable the interrupt for
    // a certain period of time once a push button is pressed
    // and then enable.    
    CNEN1bits.CN0IE = 1;
    CNEN1bits.CN11IE = 1;
    CNEN2bits.CN30IE = 1;
    // Enable the interrupt.
    IEC1bits.CNIE = 1;
}