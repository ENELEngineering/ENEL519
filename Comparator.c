/*
 * File:   Comparator.c
 * Author: johns
 *
 * Created on October 27, 2023, 5:26 PM
 */

#include "xc.h"
#include "UART2.h"

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
            CVRR_value = 1;
            CVR_value = roundf(cvrr_one);
            CVRCONbits.CVRR = CVRR_value;
            CVRCONbits.CVR = CVR_value;
            
        } else {
            CVRR_value = 0;
            CVR_value = roundf(cvrr_zero);
            CVRCONbits.CVRR = CVRR_value;
            CVRCONbits.CVR = CVR_value;
            
        }
    }
    
    // Display the final states.
    Disp2String("CVRR = ");
    Disp2Dec(CVRR_value);
    Disp2String("CVR = ");
    Disp2Dec(CVR_value);
    Disp2String("\n \r");
}
