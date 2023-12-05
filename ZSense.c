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
#include "comparator.h"
#include "Timer.h"
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

/**
 * Allow comparator to interrupt with a time window of 1 second. 
 * If the comparator does not interrupt within 1 second, then the ADC operation
 * is used to find the capacitance. 
 */
void CSense(void) {
    uint16_t comparator_interrupt = 0;
    float dt = 0;
    float dv = 0;
    
    /****** Start timer 2 to measure 1 second ******/
    T3CONbits.TON = 1; // Start the 16 bit timer 2.
    TMR3 = 0; // Clear timer 2 at the start.
    
    /****** Start the current ******/
    // 0.00uA => 0, 0.55uA => 1, 5.5uA => 2, 55uA => 3
    CTMUinit(2);
    start_current();
    
    // 15625 represents 1 second on a 8MHz clock with 1:256 PreScaler.
    while (TMR3 < 15625) {
        // Check if interrupt occurred in comparator, method returns 1 meaning interrupt occurred, otherwise it returns 0.
        comparator_interrupt = check_interrupt();
        
        // The timer is turned off, dT is measured, dV is 2V for CVRef set. 
        if (comparator_interrupt == 1) {
            T3CONbits.TON = 0; // Stop the 16 bit timer 2.
            dt = TMR3 * 512 / (8E6);
            dv = 2.00;
            break;
        }
    }
    
    reset_interrupt();
    
    // Check if the Timer is on.
    if (T3CONbits.TON == 1) {
        T3CONbits.TON = 0; // Stop the 16 bit timer 2.
        
        // Retrieve ADC value for voltage, get dT from PR.
        uint16_t ADCvalue = do_ADC();
        dv = (ADCvalue * 3.25/pow(2,10));
        dt = TMR3 * 512 / (8E6);
    }
    
    // Stop the current
    stop_current();
    TMR3 = 0;
    
    // Calculate capacitance
    float capacitance = compute_capacitance(dv, 5.5E-6, dt);
    capacitance /= 1E-6;
    dt /= 1E-3;
    dv /= 1E-6;
    
    Nop();
    
    Disp2String("\r Voltage: ");
    Disp2Float(dv);
    Disp2String("uV");
    
    Disp2String(" Time: ");
    Disp2Float(dt);
    Disp2String("ms");
    
    Disp2String(" Capacitance: ");
    Disp2Float(capacitance);
    Disp2String("uF\n\r");
    return;
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

/**
 * This computes the capacitance value.
 * @param voltage The change of voltage based on the change in time in Volts.
 * @param current The instantaneous or the RMS current set for the CTMU in Amperes.
 * @param time The change of time in seconds. 
 * @return The capacitance value in Farads.
 */
float compute_capacitance(float voltage, float current, float time) {
    return current * time/voltage;
}