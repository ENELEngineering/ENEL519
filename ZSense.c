/*
 * File:   ZSense.c
 * Author: Owner
 *
 * Created on November 16, 2023, 12:27 PM
 */

#include <math.h>
#include "xc.h"
#include "UART2.h"
#include "ZSense.h"
#include "CTMU.h"
#include "ADC.h"

/**
 * Measure the resistance value depending on the voltage 
 * and the current supplied at the pin.
 * OHM's law: R = V/I
 */
void RSense(void) {
    
    float known_current = 5.5E-6;
    float known_resistance = 99790;
    
    // 0.00uA => 0, 0.55uA => 1, 5.5uA => 2, 55uA => 3
    CTMUinit(2);
    start_current();
    uint16_t ADCvalue = do_ADC();
    stop_current();
    
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

void CSense(void) {
    
}

/**
 * Starts the current CTMU source at AN5. 
 * Also ensures the source is not grounded. 
 */
void start_current(void) {
    // Start current
    CTMUCONbits.IDISSEN = 0; // Analog current source output is not grounded.
    // 10, or 01 for SW control of current source is turned on.
    CTMUCONbits.EDG1STAT = 1; // Edge 1 event has occurred.
    CTMUCONbits.EDG2STAT = 0; // Edge 2 event has not occurred.
}

/**
 * Stop the current CTMU source at AN5.
 * Also ensures to ground the remaining current.
 */
void stop_current(void) {
   // Stop current
    CTMUCONbits.EDG1STAT = 1; 
    CTMUCONbits.EDG2STAT = 1;  
    CTMUCONbits.IDISSEN = 1; // Analog current source output is grounded.
}
