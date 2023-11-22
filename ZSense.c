/*
 * File:   ZSense.c
 * Author: Owner
 *
 * Created on November 16, 2023, 12:27 PM
 */

#include <math.h>
#include "xc.h"
#include "ZSense.h"
#include "Timer.h"
#include "UART2.h"


/* Configure CTMU settings configurations and initializations.
 * @param current_bits: 0=>current source is disabled
 *                      1=>0.55uA nominal current source
 *                      2=>5.5uA 10 x base current
 *                      3=>55uA 100 x base current
 */
void CTMUinit(void) {
    CTMUCONbits.CTMUEN = 1; // Enable CTMU
    //CTMUCONbits.CTMUSIDL = 0; // Continue module operation in idle mode.
    CTMUCONbits.TGEN = 0; // Disable edge delay generation for SW control.
    CTMUCONbits.EDGEN = 0; // Edges are blocked for SW control.
    CTMUCONbits.EDGSEQEN = 0; // No edge sequence is needed for SW control.
    CTMUCONbits.IDISSEN = 0; // Analog current source output is not grounded
    CTMUCONbits.CTTRIG = 0; // Trigger output is disabled for SW control.
    
    // 10, or 01 for SW control of current source is turned on.
    CTMUCONbits.EDG1STAT = 1; // Edge 1 event has occurred.
    CTMUCONbits.EDG2STAT = 0; // Edge 2 event has not occurred.
   
    CTMUICONbits.ITRIM = 0b000000; // Nominal current output specified by IRNG<1:0>
    CTMUICONbits.IRNG = 2; // for 5.5 uA  
    return;
}

/*
 * Provide ADC settings configuration and initializations.
 * Set bits in AD1CON1 register.
 */
void configure_ADC_AD1CON1(void) {
    AD1CON1bits.ADON = 1; // A/D converter module is operating for current.
    AD1CON1bits.ADSIDL = 0; // Continue module operation in idle mode.
    AD1CON1bits.FORM = 0b00; // Data output format should be saved as an integer.
    AD1CON1bits.SSRC = 0b000; // Clearing SAMP bit ends sampling and starts conversion.
    AD1CON1bits.ASAM = 0; // Sampling begins when SAMP bit is set.
    // Clear the A/D Conversion status bit.
    AD1CON1bits.DONE = 1; // A/D conversion is done.
    return;
}

/*
 * Provide ADC settings configuration and initializations.
 * Set bits in AD1CON2 register.
 */
void configure_ADC_AD1CON2(void) {
    AD1CON2bits.VCFG = 0b000; // Voltage reference configuration VR+ = AVdd and VR- = AVss.
    AD1CON2bits.CSCNA = 0; // Do not scan inputs.
    // Sample/convert sequences per interrupt selection bits.
    //AD1CON2bits.SMPI = 0b0000; // Interrupts at the completion of conversion for each sample/convert sequence.
    AD1CON2bits.BUFM = 0; // Buffer configured as one 16-word buffer.
    AD1CON2bits.ALTS = 0; // Always use MUX A input multiplexer settings.
    return;
}

/*
 * Provide ADC settings configuration and initializations.
 * Set bits in AD1CON3 register.
 */
void configure_ADC_AD1CON3(void) {
    AD1CON3bits.ADRC = 0; // Clock derived from the system clock.
    AD1CON3bits.SAMC = 0b11111; // Slowest sampling time = 31*2/fclk.
    return;
}

/**
 * Perform ADC conversion at AN11
 * @return 10-bit ADC output
 */
unsigned int do_ADC(void) {
    uint16_t ADCvalue; // 16 bit register used to hold ADC converted digital output ADC1BUFO.
    
    /*---------- ADC INITIALIZATION ----------*/
    configure_ADC_AD1CON1();
    configure_ADC_AD1CON2();
    // Configure the ADC's sample time by setting bits in AD1CON3.
    // Ensure sample time is 1/10th of signal being sampled or as per application's speed and needs.
    configure_ADC_AD1CON3();
    
    // Select and configure ADC input.
    AD1CHSbits.CH0NA = 0; // Channel 0 negative input is VR-.
    AD1CHSbits.CH0SA = 0b1011; // Channel 0 positive input is AN11.
    AD1PCFGbits.PCFG11 = 0; // Pin configured in Analog mode; I/O port read disabled; A/D samples pin voltage.
    AD1CSSLbits.CSSL11 = 0; // Analog channel omitted from input scan.
    TRISBbits.TRISB13 = 1; // Analog pin to measure voltage is set as an input.
    
     /*---------- ADC SAMPLING AND CONVERSION - CAN BE A DIFFERENT FUNCTION ----------*/
    AD1CON1bits.SAMP = 1; // A/D sample/hold amplifier is sampling input.
    
//    while(AD1CON1bits.DONE==0) { 
//        //delay_sec(1, idle_on);
//    }
    ADCvalue = ADC1BUF0; // ADC output is stored in ADC1BUF0 as this point.
    
    AD1CON1bits.SAMP = 0; // Stop sampling
    AD1CON1bits.ADON=0; // Turn off ADC, ADC value stored in ADC1BUFO.
    AD1CON1bits.DONE = 1; // A/D conversion is done.
    
    // Stop current
    CTMUCONbits.EDG1STAT = 1; 
    CTMUCONbits.EDG2STAT = 1; 
    return (ADCvalue); // Returns 10 bit ADC output stored in ADC1BIFO to calling function.
}

/**
 * Measure the resistance value depending on the voltage and the current supplied 
 * at the pin.
 * OHM's law: R = V/I
 */
void RSense(void) {
    
    CTMUinit();
    uint16_t ADCvalue = do_ADC();
    
    float current = 5.5E-6;
    float voltage = ADCvalue * 3.25/pow(2,10);
    float resistance = (voltage/current)/1000; // Convert to KOhms
    
    Disp2String("\r Voltage: ");
    Disp2Float(voltage);
    Disp2String(" Resistance: ");
    Disp2Float(resistance);
    Disp2String(" KOhms \n\r");
    
    return;
}
