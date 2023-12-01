/*
 * File:   ZSense.c
 * Author: Owner
 *
 * Created on November 16, 2023, 12:27 PM
 */

#include <libpic30.h>
#include <math.h>
#include "xc.h"
#include "ZSense.h"
#include "ADC.h"
#include "Timer.h"
#include "UART2.h"

#define FCY 16000UL

/* Configure CTMU settings configurations and initializations.
 * @param current_bits: 0=>current source is disabled
 *                      1=>0.55uA nominal current source
 *                      2=>5.5uA 10 x base current
 *                      3=>55uA 100 x base current
 */
void CTMUinit(uint8_t current_bits) {
    CTMUCONbits.CTMUEN = 1; // Enable CTMU
    CTMUCONbits.CTMUSIDL = 0; // Continue module operation in idle mode.
    CTMUCONbits.TGEN = 0; // Disable edge delay generation for SW control.
    CTMUCONbits.EDGEN = 0; // Edges are blocked for SW control.
    CTMUCONbits.EDGSEQEN = 0; // No edge sequence is needed for SW control.
    CTMUCONbits.IDISSEN = 0; // Analog current source output is not grounded
    CTMUCONbits.CTTRIG = 0; // Trigger output is disabled for SW control.
    
    // 10, or 01 for SW control of current source is turned on.
    CTMUCONbits.EDG1STAT = 1; // Edge 1 event has occurred.
    CTMUCONbits.EDG2STAT = 0; // Edge 2 event has not occurred.
   
    CTMUICONbits.ITRIM = 0b111110; // Nominal current output specified by IRNG<1:0>
    CTMUICONbits.IRNG = current_bits; // for 5.5 uA  
    return;
}

/**
 * Measure the resistance value depending on the voltage and the current supplied 
 * at the pin.
 * OHM's law: R = V/I
 */
void RSense(void) {
    
    float known_current = 5.5E-6;
    float known_resistance = 99790;
            
    if (known_current == 55E-6) {
        CTMUinit(3);
    }
    else if (known_current == 5.5E-6) {
        CTMUinit(2);
    }
    else if (known_current == 0.55E-6) {
        CTMUinit(1);
    } 
    else {
        CTMUinit(0);
    }
    
    uint16_t ADCvalue = do_ADC();
    
    float voltage = ADCvalue * 3.25/pow(2,10);
    float resistance = (voltage/known_current)/1000; // Convert to KOhms
    float current = (voltage/known_resistance)*1000000; // Convert to uA
    
    Disp2String("\r Voltage (meas): ");
    Disp2Float(voltage);
    Disp2String("V");
    Disp2String(" Resistance (calc): ");
    Disp2Float(resistance);
    Disp2String("KOhms");
    Disp2String(" Current (calc): ");
    Disp2Float(current);
    Disp2String("uA\n\r");
    return;
}
