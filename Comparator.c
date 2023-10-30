/*
 * File:   Comparator.c
 * Author: johns
 *
 * Created on October 27, 2023, 5:26 PM
 */

#include "xc.h"

/**
 * Performs comparator voltage division of the source voltage
 * to output the voltage defined as the Vref. 
 * @param vref The voltage to output at PIN17 which is the CVREF pin.
 */
void CVREFInit(float vref) {
    float CVRSRC = 3.0;
    if (vref < 0.83) {
        CVRCONbits.CVRR = 1;
        CVRCONbits.CVR = roundf(24*vref / CVRSRC);
    }
    else if (vref > 2.06) {
        CVRCONbits.CVRR = 0;
        CVRCONbits.CVR = roundf(32*(vref - (CVRSRC/4)) / CVRSRC);
    } else {
        float cvrr_one = 24*vref / CVRSRC;
        float cvrr_one_remainder = cvrr_one - (uint16_t) cvrr_one;
        
        float cvrr_zero = 32*(vref - (CVRSRC/4)) / CVRSRC;
        float cvrr_zero_remainder = cvrr_zero - (uint16_t) cvrr_zero;
        
        if (cvrr_one_remainder < cvrr_zero_remainder) {
            CVRCONbits.CVRR = 1;
            CVRCONbits.CVR = roundf(cvrr_one);
        } else {
            CVRCONbits.CVRR = 0;
            CVRCONbits.CVR = roundf(cvrr_zero);
        }
    }
}
