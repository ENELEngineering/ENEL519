/*
 * File:   comparator.c
 * Author: johns
 *
 * Created on November 10, 2023, 3:51 PM
 */

#include "xc.h"

void __attribute__((interrupt, no_auto_psv))__CompInterrupt(void) {
    
    Disp2String("\n\r I am interrupted \n");
    
    if (IFS1bits.CMIF == 1) {
        // If interrupt is due to comparator 1.
        if (CMSTATbits.C1OUT == 1) {
            Disp2String("\n\r C1out hi\n");
        } else {
            Disp2String("\n\r C1out low\n");
        }
        
        // If interrupt is due to comparator 2.
        if (CMSTATbits.C2OUT == 1) {
            Disp2String("\n\r C2out hi\n");
        } else {
            Disp2String("\n\r C2out low\n");
        }
    }
    IFS1bits.CMIF = 0; // Clear the IF flag.
    CM2CONbits.CEVT = 0; // Interrupts disabled till this bit is cleared.
    Nop();
}

/*
 * Comparator Setup 
 */
void ComparatorInit(void) {
    
    // Comparator 1 and 2 enable.
    CM1CONbits.CON = 1; 
    CM2CONbits.CON = 1; 
    
    // Comparator 1 and 2 output enable.
    CM1CONbits.COE = 1; 
    CM2CONbits.COE = 1; 
    
    // Comparator 1 and 2 output is not inverted.
    CM1CONbits.CPOL = 0; 
    CM2CONbits.CPOL = 0;
    
    // Comparator 1 and 2 does not operate in low power mode.
    CM1CONbits.CLPWR = 0;
    CM2CONbits.CLPWR = 0;
    
    // Comparator event has occurred. 
    CM1CONbits.CEVT = 1; 
    CM2CONbits.CEVT = 1; 
    
    // Trigger/event/interrupt generated on any change of the comparator output.
    CM1CONbits.EVPOL = 0b11; 
    CM2CONbits.EVPOL = 0b11; 
    
    // Comparator non inverting input connects to internal CVRef voltage.
    CM1CONbits.CREF = 1;
    CM2CONbits.CREF = 1;
    
    // Inverting input of comparator connects to CxINC pin.
    CM1CONbits.CCH = 0b01;
    CM2CONbits.CCH = 0b01;
    
    // Continue operation of all enabled comparators in idle mode.
    CMSTATbits.CMIDL = 0; 
    
    IPC4bits.CMIP = 0b111; // Set priority to level 7
    IEC1bits.CMIE = 1;  // Enable Comparator Interrupt
    
    // CMSTATbits.C2EVT <= Comparator 2 event status bit is a read only.
    // CMSTATbits.C2OUT <= Comparator 2 output status but is a read only.
    
}