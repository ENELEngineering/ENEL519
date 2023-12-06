/*
 * File:   ADC.c
 * Author: Owner
 *
 * Created on November 30, 2023, 5:43 PM
 */

#include "xc.h"
#include "ADC.h"
#include "Timer.h"

/*
 * Provide ADC settings configuration and initializations.
 * Set bits in AD1CON1 register.
 */
void configure_ADC_AD1CON1(void) {
    AD1CON1bits.ADON = 1; // A/D converter module is operating for current.
    //AD1CON1bits.ADSIDL = 0; // Continue module operation in idle mode.
    AD1CON1bits.FORM = 0b00; // Data output format should be saved as an integer.
    AD1CON1bits.SSRC = 0b111; // Clearing SAMP bit ends sampling and starts conversion.
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
    AD1CON2bits.SMPI = 0b0000; // Interrupts at the completion of conversion for each sample/convert sequence.
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
    AD1CON3bits.SAMC = 0b11111;  // Auto-sample Time bits 32 TAD - 32*2/fclk
    return;
}

/**
 * ADC configuration
 */
void ADCinit(void) {
    /*---------- ADC INITIALIZATION ----------*/
    configure_ADC_AD1CON1();
    configure_ADC_AD1CON2();
    // Configure the ADC's sample time by setting bits in AD1CON3.
    // Ensure sample time is 1/10th of signal being sampled or as per application's speed and needs.
    configure_ADC_AD1CON3();
    
    // Select and configure ADC input.
    AD1CHSbits.CH0NA = 0; // Channel 0 negative input is VR-.
    AD1CHSbits.CH0SA = 0b0101; // Channel 0 positive input is AN5.
    AD1PCFGbits.PCFG5 = 0; // Pin configured in Analog mode; I/O port read disabled; A/D samples pin voltage.
    AD1CSSL = 0; // Corresponding analog channel is selected for input scan.
    //AD1CSSLbits.CSSL11 = 0; // Analog channel omitted from input scan.
    TRISAbits.TRISA3 = 1; // Analog pin to measure voltage is set as an input.
    //TRISBbits.TRISB13 = 1;
}

/**
 * Perform ADC conversion at AN11
 * @return 10-bit ADC output
 */
unsigned int do_ADC(void) {
    uint16_t ADCvalue; // 16 bit register used to hold ADC converted digital output ADC1BUFO.
    
    ADCinit();
    /*---------- ADC SAMPLING AND CONVERSION - CAN BE A DIFFERENT FUNCTION ----------*/
    AD1CON1bits.SAMP = 1; // A/D sample/hold amplifier is sampling input.
    //AD1CON1bits.DONE = 0; // A/D conversion is not done.
    while(AD1CON1bits.DONE==0) { 
        delay_ms(1000, 0);
    }
    
    ADCvalue = ADC1BUF0; // ADC output is stored in ADC1BUF0 as this point. 
    // Stop ADC sampling
    AD1CON1bits.SAMP = 0; // Stop sampling
    AD1CON1bits.ADON = 0; // Turn off ADC, ADC value stored in ADC1BUFO.
    //AD1CON1bits.DONE = 1; // A/D conversion is done.
    
    return (ADCvalue); // Returns 10 bit ADC output stored in ADC1BIFO to calling function.
}
