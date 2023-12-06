/*
 * File:   CTMU.c
 * Author: johns
 *
 * Created on December 4, 2023, 2:21 PM
 */

#include "xc.h"
#include "CTMU.h"

/* Configure CTMU settings configurations and initializations.
 * @param current_bits: 0=>current source is disabled
 *                      1=>0.55uA nominal current source
 *                      2=>5.5uA 10 x base current
 *                      3=>55uA 100 x base current
 */
void CTMUinit(uint8_t current_bits) {
    CTMUCONbits.CTMUEN = 0; // Turn off CTMU
    CTMUCONbits.CTMUSIDL = 0; // Continue module operation in idle mode.
    CTMUCONbits.TGEN = 0; // Disable edge delay generation for SW control.
    CTMUCONbits.EDGEN = 0; // Edges are blocked for SW control.
    CTMUCONbits.EDGSEQEN = 0; // No edge sequence is needed for SW control.
    CTMUCONbits.IDISSEN = 0; // Analog current source output is not grounded
    CTMUCONbits.CTTRIG = 0; // Trigger output is disabled for SW control.
    
    CTMUCONbits.EDG2STAT = 0;  // Edge 2 Status bit event has not occurred
    CTMUCONbits.EDG1STAT = 0;  // Edge 1 Status bit event has not occurred
   
    CTMUICONbits.ITRIM = 0b111110; // Nominal current output specified by IRNG<1:0>
    CTMUICONbits.IRNG = current_bits; // Current value set .
    return;
}