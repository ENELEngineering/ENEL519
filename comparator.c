/*
 * File:   comparator.c
 * Author: johns
 *
 * Created on November 10, 2023, 3:51 PM
 */

#include "xc.h"
#include "UART2.h"
#include "comparator.h"

uint16_t ninterrupts = 0;
uint16_t previous_c2out = 0;

void __attribute__((interrupt, no_auto_psv)) _CompInterrupt(void) {
    if (IFS1bits.CMIF == 1) {
        ninterrupts += 1;
    }
    
    // If interrupt is due to comparator 2.
    if (CMSTATbits.C2OUT == 1) {
        // Only display messages once. 
        if (ninterrupts <= 1) {
            // CPOL is 1 which means CREF < C2INC.
            Disp2String("C2out hi \r");
        }
    }
    else {
        // Only display messages once.
        if (ninterrupts <= 1) {
            // CPOL is 1 which means CREF > C2INC
            Disp2String("C2out lo\r");
        }
    }
    
    // This condition means there's been a change of state, thus reset the number of interrupts.
    if (previous_c2out != CMSTATbits.C2OUT) {
        ninterrupts = 0;
    }
    // Record the previous value of C2Out.
    previous_c2out = CMSTATbits.C2OUT;
    
    IFS1bits.CMIF = 0; // Clear the IF flag.
    CM2CONbits.CEVT = 0; // Interrupts disabled till this bit is cleared.
    Nop();
}

/*
 * Comparator Setup 
 */
void ComparatorInit(void) {
    //CM2CONbits.CON = 0; // Comparator 2 enable.
    CM2CONbits.CON = 1; // Comparator 2 enable.
    CM2CONbits.COE = 1; // Comparator 2 output enable.
    CM2CONbits.CPOL = 1; // Comparator 2 output is inverted.
    CM2CONbits.CLPWR = 0; // Comparator 2 does not operate in low power mode.
    
    CM2CONbits.EVPOL = 3; // Trigger/event/interrupt generated on any change of the comparator output.
    CM2CONbits.CREF = 1; // Comparator non inverting input connects to internal CVRef voltage.
    CM2CONbits.CCH = 0b01; // Inverting input of comparator connects to CxINC pin.
    CMSTATbits.CMIDL = 0; // Continue operation of all enabled comparators in idle mode.
    CM2CONbits.CEVT = 0; // Comparator event bit.
    
    IPC4bits.CMIP = 1; // Set priority to level 7
    IEC1bits.CMIE = 1;  // Enable Comparator Interrupt 
    return;
}

/**
 * Performs comparator voltage division of the source voltage
 * to output the voltage defined as the Vref. 
 * @param vref The voltage to output at PIN17 which is the CVREF pin.
 */
void CVREFInit(float vref) {
    float CVRSRC = 3.0; // Microcontroller input voltage.
    uint16_t CVR_value = 0; // Comparator vref value selection bits.
    uint16_t CVRR_value = 0; // Comparator vref range selection bit.
    
    // CVRR is 1 for vref below 0.83 threshold.
    if (vref < 0.83) {
        CVRR_value = 1;
        CVR_value = roundf(24*vref / CVRSRC);
        CVRCONbits.CVRR = CVRR_value;
        CVRCONbits.CVR = CVR_value;
    }
    // CVRR is 0 for vref higher 2.06 threshold.
    else if (vref > 2.06) {
        CVRR_value = 0;
        CVR_value = roundf(32*(vref - (CVRSRC/4)) / CVRSRC);
        CVRCONbits.CVRR = CVRR_value;
        CVRCONbits.CVR = CVR_value;
    } 
    // Comparing based on the lower remainder in the CVRR value.
    else {
        float cvrr_one = 24*vref / CVRSRC;
        float cvrr_one_remainder = cvrr_one - (uint16_t) cvrr_one;
        
        float cvrr_zero = 32*(vref - (CVRSRC/4)) / CVRSRC;
        float cvrr_zero_remainder = cvrr_zero - (uint16_t) cvrr_zero;
        
        if (cvrr_one_remainder < cvrr_zero_remainder) {
            
            if ((uint16_t) cvrr_one > 15) {
                CVRR_value = 0;
                CVR_value = roundf(cvrr_zero);
            } else {
                CVRR_value = 1;
                CVR_value = roundf(cvrr_one);
            }
            
            CVRCONbits.CVRR = CVRR_value;
            CVRCONbits.CVR = CVR_value;
            
        } else {
            
            if ( (uint16_t) cvrr_zero > 15) {
                CVRR_value = 1;
                CVR_value = roundf(cvrr_one);
            } else {
                CVRR_value = 0;
                CVR_value = roundf(cvrr_zero);
            }
            
            CVRCONbits.CVRR = CVRR_value;
            CVRCONbits.CVR = CVR_value;
            
        }
    }
    return;
}