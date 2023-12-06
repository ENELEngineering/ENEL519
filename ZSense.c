/*
 * File:   ZSense.c
 * Author: Owner
 *
 * Created on November 16, 2023, 12:27 PM
 */

#include <libpic30.h>
#include <math.h>
#include "xc.h"
#include "UART2.h"
#include "ZSense.h"
#include "ADC.h"
#include "comparator.h"
#include "Timer.h"

#define FCY 4000000UL

/**
 * Measure the resistance value depending on the voltage 
 * and the current supplied at the pin.
 * OHM's law: R = V/I
 */
void RSense(void) {
    
    /****** Drain the charge on the circuit ******/
    CTMUCONbits.CTMUEN = 1;  // Turn on CTMU.
    CTMUCONbits.IDISSEN = 1; // Drain charge on the circuit.
    
    /****** Delay for 260 ms ******/
    delay_ms(260, 0);
    CTMUCONbits.IDISSEN = 0; // End drain of circuit.
    
    /****** Start the charge on the circuit ******/
    start_current();
    uint16_t ADCvalue = do_ADC();
    stop_current();
    
    float voltage = ADCvalue * 3.25/pow(2,10);
    float resistance = compute_resistance(voltage, 5.5E-6);
    resistance /= 1000; // Convert to KOhms
    
    Disp2String("\r Voltage: ");
    Disp2Float(voltage);
    Disp2String("V");
    Disp2String(" Resistance: ");
    Disp2Float(resistance);
    Disp2String("KOhms");
    return;
}

/**
 * Allow comparator to interrupt with a time window of 1 second. 
 * If the comparator does not interrupt within 1 second, then the ADC operation
 * is used to find the capacitance. 
 */
void CSense(void) {
    
    float capacitance = 0;
    float start_voltage = 0;
    float final_voltage = 0;
    float dt = 0;
    float dv = 0;
    float di = 0;
    float pr3 = 0;
    
    uint16_t comparator_interrupt = 0;
    uint16_t ADCvalue = 0;
    
    /****** Drain the charge on the circuit ******/
    CTMUCONbits.CTMUEN = 1;  // Turn on CTMU.
    CTMUCONbits.IDISSEN = 1; // Drain charge on the circuit.
    
    /****** Delay for 260 ms ******/
    delay_sec(1, 0);
    ADCvalue = do_ADC();
    start_voltage = (ADCvalue * 3.25/pow(2,10));
    
    /****** Use Timer 3 to measure comparator interrupt ******/
    T3CONbits.TON = 1; // Start the 16 bit timer 3.
    TMR3 = 0; // Clear timer 3 at the start.
    
    /****** Start the charge on the circuit ******/
    start_current();
    CTMUCONbits.IDISSEN = 0; // End drain of circuit.
    
    // Check for comparator to interrupt within 6ms.
    while(TMR3 <= 250) {
        // Check if interrupt occurred in comparator, method returns 1 meaning interrupt occurred, otherwise it returns 0.
        comparator_interrupt = check_interrupt();
        // The timer is turned off, dT is measured, dV is 2V for CVRef set. 
        if (comparator_interrupt == 1) {
            pr3 = TMR3; // Conversion to float such that Hex values is not used.
            
            // The comparator interrupted too soon, let ADC handle the computation.
            if (pr3 == 0) {
                continue;
            }
            
            stop_current(); // Stop the current.  
            T3CONbits.TON = 0; // Stop the 16 bit Timer 3.

            dt = pr3 * 512 / (8E6);
            //final_voltage = 2.16;
            ADCvalue = do_ADC();
            final_voltage = (ADCvalue * 3.25/pow(2,10));
            break;
        }
    }
    
    /****** Use ADC if comparator never interrupts ******/
    // Check if the Timer is on.
    if (T3CONbits.TON == 1) {
        pr3 = TMR3;
        stop_current(); // Stop the current
        T3CONbits.TON = 0; // Stop the 16 bit timer 3.
        
        // Retrieve ADC value for voltage, get dT from PR.
        ADCvalue = do_ADC();
        final_voltage = (ADCvalue * 3.25/pow(2,10));
        dt = pr3 * 2 / (8E6);
        
    }
    
    reset_interrupt(); // comparator interrupt is set back to 0.
    TMR3 = 0; // Clear Timer 3
    
    /****** Find the capacitance ******/
    dv = fabs(final_voltage - start_voltage);
    di = 5.5E-6; // Preset in the CTMU.
    capacitance = compute_capacitance(dv, di, dt);
   
    capacitance /= 1E-6;
    dt /= 1E-3;
    
    Disp2String("\r Capacitance: ");
    Disp2Float(capacitance);
    Disp2String("uF\r");
    return;
}


/**
 * Starts the current CTMU source at AN5. 
 * Also ensures the source is not grounded. 
 */
void start_current(void) {
    // Start current
    CTMUCONbits.EDG1STAT = 1; // Edge 1 event has occurred.
}

/**
 * Stop the current CTMU source at AN5.
 * Also ensures to ground the remaining current.
 */
void stop_current(void) {
   // Stop current
    CTMUCONbits.EDG1STAT = 0; 
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

/**
 * This computes the resistance value.
 * @param voltage The voltage drop across the resistor.
 * @param current The current flowing through the resistor.
 * @return The resistance value in Ohms across the resistor.
 */
float compute_resistance(float voltage, float current) {
    return voltage/current;
}