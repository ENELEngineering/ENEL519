/*
 * File:   ZSense.c
 * Author: Owner
 *
 * Created on November 16, 2023, 12:27 PM
 */


#include "xc.h"

void CTMUinit(void) {
    
    CTMUCONbits.CTMUEN = 1;
    CTMUCONbits.CTMUSIDL = 0;
    CTMUCONbits.TGEN = 0;
    CTMUCONbits.EDGEN = 0;
    CTMUCONbits.EDGSEQEN = 0;
    CTMUCONbits.IDISSEN = 0;
    CTMUCONbits.CTTRIG = 0;
    CTMUCONbits.EDG1STAT = 1;
    CTMUCONbits.EDG2STAT = 0;
    
    CTMUICONbits.ITRIM = 0b000000;
    CTMUICONbits.IRNG = 0b10; // for 5.5 uA
    
    AD1CON1bits.ADON = 1;
    AD1CON1bits.ADSIDL = 0;
    AD1CON1bits.FORM = 0b00;
    AD1CON1bits.SSRC = 0b000;
    AD1CON1bits.ASAM = 0;
    AD1CON1bits.SAMP = 1;
    AD1CON1bits.DONE = 1;
    
    AD1CON2bits.VCFG = 0b000;
    AD1CON2bits.CSCNA = 0;
    AD1CON2bits.BUFM = 0;
    AD1CON2bits.ALTS = 0;
    
    AD1CON3bits.ADRC = 0;
    
    AD1CHSbits.CH0NA = 0;
    AD1CHSbits.CH0SA = 0b1011;
    
    AD1PCFGbits.PCFG11 = 0;
    AD1CSSLbits.CSSL11 = 0;
    return 0;
}
