/*
 * File:   IO.c
 * Author: johns
 *
 * Created on October 2, 2023, 2:48 PM
 */

#include "xc.h"
#include "IO.h"
#include "Timer.h"

// Global Variables
uint16_t RB4_Flag, RA4_Flag, RA2_Flag;
uint16_t ninterrupt;

/* Create the ISR routine. Common interrupt routine for all CN inputs.
 * The interrupt is triggered by any change of state: hi to lo, lo to hi.
 * Interrupts will be triggered for debounces on push buttons too.
 * Clear the interrupt flag
 * In interrupt routine, check the PORTs to determine 
 * which IO pin had a change of notification.
 */
void __attribute__((interrupt, no_auto_psv))_CNInterrupt(void) {

    // Should handle debounce effects.
    IEC1bits.CNIE = 0;
    delay_ms(100, 1);
    Nop();
    
    if (IFS1bits.CNIF = 1) {
        ninterrupt += 1;
    }

    if (PORTBbits.RB4 == 0 && PORTAbits.RA4 == 0 && PORTAbits.RA2 == 0) {
        RB4_Flag = 1;
        RA4_Flag = 1;
        RA2_Flag = 1;
    }
    else if (PORTBbits.RB4 == 0 && PORTAbits.RA4 == 0 && PORTAbits.RA2 == 1) {
        RB4_Flag = 1;
        RA4_Flag = 1;
        RA2_Flag = 0;
    }
    else if (PORTBbits.RB4 == 0 && PORTAbits.RA4 == 1 && PORTAbits.RA2 == 0) {
        RB4_Flag = 1;
        RA4_Flag = 0;
        RA2_Flag = 1;
    }
    else if (PORTBbits.RB4 == 0 && PORTAbits.RA4 == 1 && PORTAbits.RA2 == 1) {
        RB4_Flag = 1;
        RA4_Flag = 0;
        RA2_Flag = 0;
    }
    else if (PORTBbits.RB4 == 1 && PORTAbits.RA4 == 0 && PORTAbits.RA2 == 0) {
        RB4_Flag = 0;
        RA4_Flag = 1;
        RA2_Flag = 1;
    }
    else if (PORTBbits.RB4 = 1 && PORTAbits.RA4 == 0 && PORTAbits.RA2 == 1) {
        RB4_Flag = 0;
        RA4_Flag = 1;
        RA2_Flag = 0;
    }
    else if (PORTBbits.RB4 = 1 && PORTAbits.RA4 == 1 && PORTAbits.RA2 == 0) {
        RB4_Flag = 0;
        RA4_Flag = 0;
        RA2_Flag = 1;
    }
    else{
        RB4_Flag = 0;
        RA4_Flag = 0;
        RA2_Flag = 0;
    }
    // Clear the IF flag.
    IFS1bits.CNIF = 0;
    IEC1bits.CNIE = 1;
}

void CN_check() {
    if (ninterrupt <= 1) {
        if (RB4_Flag == 1 && RA4_Flag == 1 && RA2_Flag == 1) {
            Disp2String("\n\r CN1/RB4, CN0/RA4, CN30/RA2 are pressed\n");
        }
        else if (RB4_Flag == 1 && RA4_Flag == 1 && RA2_Flag == 0) {
            Disp2String("\n\r CN1/RB4, CN0/RA4 are pressed\n");
        }
        else if (RB4_Flag == 1 && RA4_Flag == 0 && RA2_Flag == 1) {
            Disp2String("\n\r CN1/RB4, CN30/RA2 are pressed\n");
        }
        else if (RB4_Flag == 1 && RA4_Flag == 0 && RA2_Flag == 0) {
            Disp2String("\n\r CN1/RB4 is pressed\n");
        }
        else if (RB4_Flag == 0 && RA4_Flag == 1 && RA2_Flag == 1) {
            Disp2String("\n\r CN0/RA4, CN30/RA2 are pressed\n");
        }
        else if (RB4_Flag == 0 && RA4_Flag == 1 && RA2_Flag == 0) {
            Disp2String("\n\r CN0/RA4 is pressed\n");   
        }
        else if (RB4_Flag == 0 && RA4_Flag == 0 && RA2_Flag == 1) {
            Disp2String("\n\r CN30/RA2 is pressed\n");
        }
    }
    
    if (ninterrupt == 2) {
        ninterrupt = 0;
    }
}

/* Wakes up the PIC from idle or sleep when push buttons tied to
 * RB4/CN1, RA4/CN0, and RA2/CN30 are pushed.
 * Displays the status of the push buttons on Teraterm window when 
 * one of more buttons are pushed. "CN1/RB4 is pressed", 
 * "CN1/RB4 and CN0/RA4 are pressed."
 */
void CN_init(){ //Function to initialize all the I/O ports and the CN interrupts
    AD1PCFG = 0xFFFF; // Turn all analog pins as digital on RA2
    TRISAbits.TRISA2 = 1;
    TRISAbits.TRISA4 = 1;
    TRISBbits.TRISB4 = 1;

    CNPU2bits.CN30PUE = 1;
    CNPU1bits.CN0PUE = 1;
    CNPU1bits.CN1PUE = 1;
    
    // To handle debounce effects, disable the interrupt for
    // a certain period of time once a push button is pressed
    // and then enable. 
    CNEN1bits.CN0IE = 1;
    CNEN1bits.CN1IE = 1;
    CNEN2bits.CN30IE = 1;
    
    IPC4bits.CNIP = 0b111; // Set highest priority for interrupt
    IEC1bits.CNIE = 1; // Interrupt request enabled
}