/*
 * File:   IO.c
 * Author: johns
 *
 * Created on October 2, 2023, 2:48 PM
 */


#include "xc.h"
#include "IO.h"

/* Create the ISR routine. Common interrupt routine for all CN inputs.
 * The interrupt is triggered by any change of state: hi to lo, lo to hi.
 * Interrupts will be triggered for debounces on push buttons too.
 * Clear the interrupt flag
 * In interrupt routine, check the PORTs to determine 
 * which IO pin had a change of notification.
 */

void __attribute__((interrupt, no_auto_psv))_CNInterrupt(void) {
    if (IFS1bits.CNIF == 1) {
        // TODO: Add filter out of debounce effects.
        if (PORTBbits.RB4 == 0) {
            // IF RB4 press is interrupting.
        } else if (PORTAbits.RA4 == 0) {
            // IF RA4 press is interrupting.
        } else if (PORTAbits.RA2 == 0) {
            // IF RA2 press is interrupting.
        }
    }
    // Clear the IF flag.
    IFS1bits.CNIF = 0;
    Nop(); 
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
void CN_init(void){
    // Set the interrupt priority to highest priority.
    IPC4bits.CNIP = 0b111;
    
    // To handle debounce effects, disable the interrupt for
    // a certain period of time once a push button is pressed
    // and then enable.
    // Enable the interrupt.
    IEC1bits.CNIE = 1
    
    
}