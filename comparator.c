/*
 * File:   comparator.c
 * Author: johns
 *
 * Created on November 10, 2023, 3:51 PM
 */

#include "xc.h"

void __attribute__((interrupt, no_auto_psv))__CompInterrupt(void) {
    if (IFS1bits.CMIF == 1) {
        // If interrupt is due to comparator 1.
        if (CMSTATbits.C1OUT == 1) {
            
        }
        // If interrupt is due to comparator 2.
        if (CMSTATbits.C2OUT == 1) {
            
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
    CM1CONbits.CON = 1; // Comparator 1 enable.
    CM2CONbits.CON = 1; // Comparator 2 enable.
    
    CM1CONbits.COE = 1; // Comparator 1 output enable.
    CM2CONbits.COE = 1; // Comparator 2 output enable.
    
    // Define CPOL, CLPWR initializations...
    
    CM1CONbits.CEVT = 1; // Comparator 1 EVPOL<1:0> event has occurred.
    CM2CONbits.CEVT = 1; // Comparator 2 EVPOL<1:0> event has occurred.
    
    // Define COUT initializations...
    
    
    // Trigger/event/interrupt generated on any change of the comparator output.
    CM1CONbits.EVPOL = 0b11; 
    CM2CONbits.EVPOL = 0b11; 
    
    // Comparator non inverting input connects to internal CVRef voltage.
    CM1CONbits.CREF = 1;
    CM2CONbits.CREF = 1;
    
    // Inverting input of comparator connects to CxINC pin.
    CM1CONbits.CCH = 0b01;
    CM2CONbits.CCH = 0b01;
    
    CMSTATbits.CMIDL = 0; // Comparator stop in idle mode.
    
    // CMSTATbits.C2EVT <= Comparator 2 event status bit is a read only.
    // CMSTATbits.C2OUT <= Comparator 2 output status but is a read only.
    
    
}